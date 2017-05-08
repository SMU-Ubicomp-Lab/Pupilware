//
//  MaximumCircleFit2.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 6/20/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "MaximumCircleFit2.hpp"
#include "../Helpers/PWGraph.hpp"
#include "../Helpers/math/Snakuscules.hpp"
#include "../Helpers/CWCVHelper.hpp"
#include "../SignalProcessing/SignalProcessingHelper.hpp"

#include <numeric>
#include <algorithm>

using namespace cv;
using namespace std;

namespace pw {

    MaximumCircleFit2::MaximumCircleFit2( const string& name ):
            IPupilAlgorithm(name),
            mode(0),
            ticks(0.0f){

    }

    MaximumCircleFit2::MaximumCircleFit2( const string& name, int mode ):
            IPupilAlgorithm(name),
            mode(mode),
            ticks(0.0f){

    }

    MaximumCircleFit2::~MaximumCircleFit2()
    {

    }


    void MaximumCircleFit2::init()
    {
        window = std::make_shared<CVWindow>(getName() + " Debug");
        window->resize(500, 500);
        window->moveWindow(200,300);

    
    }

    PWPupilSize MaximumCircleFit2::process( const cv::Mat& src, const PWFaceMeta &meta )
    {
        assert(!src.empty());
        
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
        
        window->update(debugImg);
        
        this->debugImage = debugImg;

        return PWPupilSize(  leftPupilRadius/meta.getEyeDistancePx()
                           , rightPupilRadius/meta.getEyeDistancePx() );

    }

    void MaximumCircleFit2::exit()
    {
        // Clean up code here.
    }

    float MaximumCircleFit2::findPupilSize(const Mat &colorEyeFrame,
                                       cv::Point eyeCenter,
                                       Mat &debugImg) {

        vector<Mat> rgbChannels(3);
        split(colorEyeFrame, rgbChannels);

        if(rgbChannels.size() <= 0 ) return 0.0f;
        // Only use a red channel.
        Mat grayEye = rgbChannels[2];

        Mat blur;
        cv::GaussianBlur(grayEye, blur,Size(5,5), 7);

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

//            int ksize = irisRadius*2;
//            float sigma = 2;
//            Mat kernelX = getGaussianKernel(ksize, sigma);
//            Mat kernelY = getGaussianKernel(ksize, sigma);
//            Mat kernelXY = kernelX * kernelY.t();
//
//            // find min and max values in kernelXY.
//            double min;
//            double max;
//            cv::minMaxIdx(kernelXY, &min, &max);
//
//            // scale kernelXY to 0-255 range;
//            cv::Mat maskImage;
//            cv::convertScaleAbs(kernelXY, maskImage, 255 / max);
//
//            // create a rect that have the same size as the gausian kernel,
//            // locating it at the eye center.
//            cv::Rect r;
//            r.width = kernelXY.cols;
//            r.height = kernelXY.rows;
//            r.x = std::max(0,eyeCenter.x - r.width/2);
//            r.y = std::max(0,eyeCenter.y - r.height/2);
//
//        const int tx = std::fmax(eyeCenter.x - irisRadius,0);
//        const int ty = std::fmax(eyeCenter.y - irisRadius,0);
//        const int t_height = (irisRadius*2 + ty) >= blur.rows? blur.rows - eyeCenter.y :irisRadius*2;
//        const int t_width = (irisRadius*2 + tx) >= blur.cols? blur.cols - eyeCenter.x :irisRadius*2;
//        Mat iris = blur(Rect( tx, ty, t_width, t_height));
//        cv::equalizeHist(iris,iris);

//        blur(r) = blur(r) - (maskImage(cv::Rect(0,0,r.width, r.height))*0.5);

//        cw::showImage("open", iris);
//        cw::showImage("m", maskImage);

///*-------- Snakucules Method ----------*/
//        Snakuscules sn2;
//        sn2.fit(blur,               // src image
//               cPoint,             // initial seed point
//               irisRadius*0.3,   // radius
//               1.3,                // alpha
//               10                  // max iteration
//        );
//        Point pPoint = sn2.getFitCenter();
//        int pupilRadius = sn2.getInnerRadius();
//        circle( debugImg,
//                eyeCenter,
//                pupilRadius,
//                Scalar(200,0,200) );
//        circle( debugImg,
//                eyeCenter,
//                sn2.getOuterRadius(),
//                Scalar(200,0,200) );
///*-------------------------------------*/

        float newR = 0.0f;
        float maxE = -1000;
        int maxR = 1;

        std::vector<float> energys;
        std::vector<int> radians;

        if(irisRadius>5)
        {

            float f = fmax(irisRadius * 0.3f, 1.0f);


//            std::cout << ">>> start with f =" << f << std::endl;
            for (int r = 2; r < irisRadius - f; ++r) {
                size_t bigSum = 0;
                size_t bigCount = 1; //not 0 to avoid divide by zero
                size_t smallSum = 0;
                size_t smallCount = 1;

                calCircularEnergy(grayEye, eyeCenter, r + f, bigSum, bigCount);
                calCircularEnergy(grayEye, eyeCenter, r, smallSum, smallCount);

                float e1 = (bigSum - smallSum) / (double) (bigCount - smallCount);
                float e2 = smallSum / (float) smallCount;
                float e = e1 - e2;

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

            newR = fmax(0.0f,newR);

//            std::cout << "<<<--- end " << maxE << ":" << newR << ":" << maxR << std::endl;

            circle(debugImg,
                   eyeCenter,
                   maxR,
                   Scalar(200, 100, 200));
            circle(debugImg,
                   eyeCenter,
                   newR,
                   Scalar(10, 200, 200));

        }

        return newR;

    }

    float MaximumCircleFit2::calCircularEnergy(const cv::Mat& src,
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


    const cv::Mat& MaximumCircleFit2::getDebugImage() const{
        return this->debugImage;
    }

    
    void MaximumCircleFit2::setMode( int mode ){
        this->mode = mode;
    }
    
}