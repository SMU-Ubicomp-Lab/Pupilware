//
//  TwoLevelSnake.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 6/20/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "TwoLevelSnake.hpp"
#include "../Helpers/PWGraph.hpp"
#include "../Helpers/math/Snakuscules.hpp"
#include "../Helpers/CWCVHelper.hpp"
#include "../SignalProcessing/SignalProcessingHelper.hpp"

#include <numeric>
#include <algorithm>

using namespace cv;
using namespace std;

namespace pw {

    TwoLevelSnake::TwoLevelSnake( const string& name ):
            IPupilAlgorithm(name),
            mode(0),
            ticks(0.0f){

    }

    TwoLevelSnake::TwoLevelSnake( const string& name, int mode ):
            IPupilAlgorithm(name),
            mode(mode),
            ticks(0.0f){

    }

    TwoLevelSnake::~TwoLevelSnake()
    {

    }


    void TwoLevelSnake::init()
    {
        window = std::make_shared<CVWindow>(getName() + " Debug");
        window->resize(500, 500);
        window->moveWindow(200,300);

    
    }

    PWPupilSize TwoLevelSnake::process( const cv::Mat& src, const PWFaceMeta &meta )
    {
        assert(!src.empty());
        

        
        Mat debugLeftEye = meta.leftEye.clone();
        float leftPupilRadius = findPupilSize( meta.leftEye
                , Point(meta.leftEye.cols/2, meta.leftEye.rows/2)
                , debugLeftEye );


        
        Mat debugRightEye = meta.rightEye.clone();
        float rightPupilRadius = findPupilSize( meta.rightEye
                , Point(meta.rightEye.cols/2, meta.rightEye.rows/2)
                , debugRightEye );


        // draw debug image
        Mat debugImg;
        hconcat(debugLeftEye,
                debugRightEye,
                debugImg);
        
        window->update(debugImg);
        
        this->debugImage = debugImg;

        return PWPupilSize(  leftPupilRadius/meta.getEyeDistancePx()
                           , rightPupilRadius/meta.getEyeDistancePx() );

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

//        cw::showHist("hit",grayEye);

//        cv::equalizeHist(grayEye,grayEye);

        int th = cw::calDynamicThreshold( grayEye, 0.4 );

//        std::cout << th << std::endl;

        cv::threshold(grayEye, grayEye, th, 255, THRESH_TRUNC);

//        cw::showImage("thr", grayEye);

        Mat blur;
        cv::GaussianBlur(grayEye, blur,Size(11,11), 5);

//        cw::showImage("blur", blur);

/*-------- Snakucules Method ----------*/
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
        cv::GaussianBlur(grayEye, blur2,Size(5,5), 3);

        if(irisRadius>5)
        {

            float f = fmax(irisRadius * 0.2f, 1.0f);


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

//            std::cout << "<<<--- end " << maxE << ":" << newR << ":" << maxR << std::endl;

//            circle(debugImg,
//                   eyeCenter,
//                   maxR,
//                   Scalar(200, 100, 200));
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

//        cw::showImage("debug", debug, 1);

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