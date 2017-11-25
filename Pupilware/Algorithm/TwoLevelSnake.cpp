//
//  TwoLevelSnake.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 6/20/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "TwoLevelSnake.hpp"
#include "../preHeader.hpp"
#include "../Helpers/PWGraph.hpp"
#include "../Helpers/math/Snakuscules.hpp"
#include "../Helpers/CWCVHelper.hpp"
#include "../SignalProcessing/SignalProcessingHelper.hpp"

#include <numeric>
#include <algorithm>

using namespace cv;
using namespace std;

#define DEBUG_MODE

namespace pw {

    TwoLevelSnake::TwoLevelSnake( const string& name ):
            IPupilAlgorithm(name),
            searchGapPercent(0.2f),
            innerBlurKernalSize(5),
            innerSD(3),
            mode(0){

    }

    TwoLevelSnake::TwoLevelSnake( const string& name, float percentGap, int ksize, int sd ):
            IPupilAlgorithm(name),
            searchGapPercent(percentGap),
            innerBlurKernalSize(ksize),
            innerSD(sd),
            mode(0){

    }

    TwoLevelSnake::~TwoLevelSnake()
    {

    }


    void TwoLevelSnake::init()
    {
#ifdef DEBUG_MODE
        window = std::make_shared<CVWindow>(getName() + " Debug");
        window->resize(500, 500);
        window->moveWindow(200,300);
#endif
    
    }

    PWPupilSize TwoLevelSnake::process( const cv::Mat& src, const PWFaceMeta &meta )
    {
        REQUIRES(!src.empty(), "Input image must not be empty.");


        cv::Point leftEyeCenterEyeCoord( meta.getLeftEyeCenter().x - meta.getLeftEyeRect().x ,
                                         meta.getLeftEyeCenter().y - meta.getLeftEyeRect().y );

        Mat debugLeftEye = src(meta.getLeftEyeRect()).clone();
        float leftPupilRadius = findPupilSize( src(meta.getLeftEyeRect())
                , leftEyeCenterEyeCoord
                , debugLeftEye );


        cv::Point rightEyeCenterEyeCoord( meta.getRightEyeCenter().x - meta.getRightEyeRect().x ,
                                          meta.getRightEyeCenter().y - meta.getRightEyeRect().y);

        Mat debugRightEye = src(meta.getRightEyeRect()).clone();
        float rightPupilRadius = findPupilSize( src(meta.getRightEyeRect())
                , rightEyeCenterEyeCoord
                , debugRightEye );


        // draw debug image
        Mat debugImg;
        hconcat(debugLeftEye,
                debugRightEye,
                debugImg);

#ifdef DEBUG_MODE
        window->update(debugImg);
#endif
        this->debugImage = debugImg;

        return PWPupilSize(  leftPupilRadius
                           , rightPupilRadius );

    }

    void TwoLevelSnake::exit()
    {
        // Clean up code here.
    }

    float TwoLevelSnake::findPupilSize(const Mat &colorEyeFrame,
                                       cv::Point eyeCenter,
                                       Mat &debugImg) {

        vector<Mat> rgbChannels(3);
        split(colorEyeFrame, rgbChannels);

        if(rgbChannels.size() <= 0 ) return 0.0f;

        // Only use a red channel.
        Mat grayEye = rgbChannels[2];


        int th = cw::calDynamicThreshold( grayEye, 0.4 );


        cv::threshold(grayEye, grayEye, th, 255, THRESH_TRUNC);


        Mat blur;
        cv::GaussianBlur(grayEye, blur,Size(11,11), 5);


/*-------- Get eye center with Snakucules Method ----------*/
        cv::Point cPoint = eyeCenter;
        Snakuscules sn;
        sn.fit(blur,               // src image
                cPoint,             // initial seed point
                grayEye.cols*0.1,   // radius
                2.0,                // alpha
                20                  // max iteration
                );
        eyeCenter = sn.getFitCenter();
        int irisRadius = sn.getInnerRadius();
        circle( debugImg,
                eyeCenter,
                irisRadius,
                Scalar(200,200,0) );
/*-------------------------------------*/


        float newR = 0.0f;
        float maxE = -1000;
        int maxR = 1;

        std::vector<float> energys;
        std::vector<int> radians;


        Mat blur2;
        cv::GaussianBlur(grayEye, blur2, Size(innerBlurKernalSize, innerBlurKernalSize), innerSD);


        if(irisRadius>5)
        {

            float f = fmax(irisRadius * searchGapPercent, 1.0f);


            for (int r = 2; r < irisRadius - f; ++r) {
                size_t bigSum = 0;
                size_t bigCount = 1; //not 0 to avoid divide by zero
                size_t smallSum = 0;
                size_t smallCount = 1;

                calCircularEnergy(blur2, eyeCenter, r + f, bigSum, bigCount);
                calCircularEnergy(blur2, eyeCenter, r, smallSum, smallCount);

                float e1 = (bigSum - smallSum) / (double) (bigCount - smallCount);
                float e2 = smallSum / (float) smallCount;
                float e = fmax(0, e1 - e2);

//                std::cout << (double) (bigCount - smallCount) << ":" << e1 << " , " << e2 << ", " << e << std::endl;

                if (e > maxE) {
                    maxE = e;
                    maxR = r;
                }

                energys.push_back(e);
                radians.push_back(r);

            }


            float sumEnergy = 0.0f;

            if(this->mode == 0){
                sumEnergy = std::accumulate(energys.begin(), energys.end(), 0.0f);

                for (int i = 0; i < radians.size(); ++i) {
                    newR += radians[i] * energys[i] / sumEnergy;
                }


            }else{

                for (int j = 0; j < energys.size(); ++j) {
                    energys[j] = exp(energys[j]);
                    sumEnergy += energys[j];
                }
                for (int i = 0; i < radians.size(); ++i) {
                    newR += radians[i] * energys[i]/sumEnergy;
                }

            }


            newR = fmax(0.0f, newR);

            circle(debugImg,
                   eyeCenter,
                   newR,
                   Scalar(10, 200, 200));

        }

        return newR;

    }

    float TwoLevelSnake::calCircularEnergy(const cv::Mat& src,
                                              const cv::Point& center,
                                              int radius, size_t& outSum, size_t& outCount){

        Mat debug = src.clone();

        const float radiusSq = radius * radius;

        size_t sum = 0;
        size_t count = 0;

        for (int i = 0; i < src.rows; ++i) {
            for (int j = 0; j < src.cols; ++j) {
                if( cw::calDistanceSq( cv::Point(j,i), center  ) < radiusSq ){
                    auto intensity = src.ptr<uchar>(i,j);
                    sum += *intensity;
                    count++;

                    *debug.ptr<uchar>(i,j) = 255;
                }

            }
        }

        outSum = sum;
        outCount = count;

        return sum / (double)count ;
    }


    const cv::Mat& TwoLevelSnake::getDebugImage() const{
        return this->debugImage;
    }

    
    void TwoLevelSnake::setMode( int mode ){
        this->mode = mode;
    }
    
}