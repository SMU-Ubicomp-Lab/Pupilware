//
//  mdStarbust.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "PixelCount.hpp"
#include "../Helpers/math/Snakuscules.hpp"
#include "../SignalProcessing/SignalProcessingHelper.hpp"
#include "../Helpers/PWGraph.hpp"

using namespace cv;

namespace pw {

    PixelCount::PixelCount(const std::string& name):IPupilAlgorithm(name){
//        window = std::make_shared<CVWindow>(getName() + " Debug");
//        window->resize(500, 500);
//        window->moveWindow(200,300);
    }

    PixelCount::~PixelCount()
    {
        
    }

    
    void PixelCount::init()
    {

        window = std::make_shared<CVWindow>(getName() + " Debug");
        window->resize(500, 500);
        window->moveWindow(200,300);


        // intialization of KF...
        KF.init(2, 1, 0);
        KF.transitionMatrix = (Mat_<float>(2, 2) << 1, 1, 0, 1);

        setIdentity(KF.measurementMatrix);
        setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
        setIdentity(KF.measurementNoiseCov, Scalar::all(0.02));
        setIdentity(KF.errorCovPost, Scalar::all(1));

    }

    std::vector<float> leftSizes;
    std::vector<float>smoothPupilSize;

    PWPupilSize PixelCount::process( const cv::Mat& src, const PWFaceMeta &meta )
    {
//        cv::Mat leftEye = src(meta.getLeftEyeRect());
//        cv::Mat rightEye = src(meta.getRightEyeRect());
//
//        Mat debugLeftEye = leftEye.clone() ;
//        float leftEnergy = calEnergy(leftEye, meta.getLocalLeftEyeCenter(), debugLeftEye) ;
////        float leftEnergy = 0;
//
//        Mat debugRightEye;
//        float rightEnergy = calEnergy(rightEye, meta.getLocalRightEyeCenter(), debugRightEye) ;
//
//        // draw debug image
//        Mat debugImg;
//        hconcat(debugLeftEye,
//                debugRightEye,
//                debugImg);
//
//        if(!debugImg.empty()){
//            window->update(debugImg);
//
//            this->debugImage = debugImg;
//        }


        return PWPupilSize( 0.0, 0.0);
    }


    float PixelCount::calEnergy( const Mat& eye, const cv::Point& eyeCenter, cv::Mat& outDebugImage ){


        std::vector<cv::Mat> bgr_planes;
        cv::split(eye, bgr_planes);

        if(bgr_planes.size() <= 0)return 0.0f;

        cv::Mat leftEyeGray = bgr_planes[2]; //red channel;


        Mat debugImg = eye.clone();

/*---------- Snakuscules technique -------------*/
        Mat blur;
//        cv::GaussianBlur(leftEyeGray, blur, Size(3,3), 3);
        cv::GaussianBlur(leftEyeGray, blur,Size(15,15), 7);

        cv::Point cPoint = eyeCenter;

        Snakuscules sn;
        sn.fit(blur,               // src image
               cPoint,             // initial seed point
               leftEyeGray.cols*0.1,   // radius
               2.0,                // alpha
               20                  // max iteration
        );
/*----------------------------------------------*/

        const float irisRadius = sn.getInnerRadius();
        const Point ec = sn.getFitCenter();

        circle( debugImg,
                eyeCenter,
                irisRadius,
                Scalar(200,200,0) );

        outDebugImage = debugImg;

//        cv::Mat mask_mat = cv::Mat::zeros(eye.rows, eye.cols, CV_8UC1); // Already created
//        cv::Mat irisMat;  // New and empty

//        circle( mask_mat, ec, irisRadius,Scalar(255), -1);

//        leftEyeGray.copyTo(irisMat, mask_mat);

        const int tx = std::fmax(ec.x - irisRadius,0);
        const int ty = std::fmax(ec.y - irisRadius,0);
        const int thi = (irisRadius*2 + ty) > leftEyeGray.rows? leftEyeGray.rows - ec.y :irisRadius*2;
        Mat r = leftEyeGray(Rect( tx, ty,
                                  irisRadius*2, thi));
//
//        cw::showImage("mask", mask_mat);
//        equalizeHist(irisMat,irisMat);
        Mat r2;
        equalizeHist(r,r2);
//        cw::showImage("ratinaH", r);
//        cw::showHist("hist", r);

        cw::openOperation(r2,r2);
//        cw::showImage("ratinaH2", r2);
//        cw::showHist("hist2", r2);



        const float sq = irisRadius*irisRadius;
        size_t sum = 0;

        for (int i = 0; i < r.rows; ++i) {
            for (int j = 0; j < r.cols; ++j) {
                sum += *r.ptr<unsigned char>(i,j);
            }
        }

        return (sum/(double)(r.rows * r.cols));
    }

    void PixelCount::exit()
    {
        std::cout << " Close my algorithm " << std::endl;
    }
}