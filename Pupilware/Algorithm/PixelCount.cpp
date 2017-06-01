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


        landmark.loadLandmarkFile("/Users/redeian/Documents/projects/Pupilware/Pupilware/shape_predictor_68_face_landmarks.dat");

    }


    PWPupilSize PixelCount::process( const cv::Mat& src, const PWFaceMeta &meta )
    {

        Mat leftEye, rightEye;

        std::vector<cv::Point> landmarkPoints;
        landmark.currentFrame = meta.getFrameNumber();
        landmark.searchLandMark(src, landmarkPoints, meta.getFaceRect(), leftEye, rightEye);

        return PWPupilSize( 0.0, 0.0);
    }


    float PixelCount::calEnergy( const Mat& eye, const cv::Point& eyeCenter, cv::Mat& outDebugImage ){


        return 0;
    }

    void PixelCount::exit()
    {
        std::cout << " Close my algorithm " << std::endl;
    }
}