//
//  Pupilware.hpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#ifndef Pupilware_hpp
#define Pupilware_hpp

#include <string>
#include <opencv2/opencv.hpp>

#include "Algorithm/PWAlgorithm.hpp"

namespace pw {

    class Pupilware {

    public:
        Pupilware();

        ~Pupilware();

        void loadVideo(const std::string &videoFilePath);

        void setAlgorithm(PWAlgorithm *algorithm);

        void execute();

    private:
        cv::VideoCapture capture;
        cv::CascadeClassifier face_cascade;

        PWAlgorithm *algorithm;

        bool findFace(const cv::Mat grayFrame, cv::Rect &outFaceRect);

        void extractEyes(cv::Rect faceROI, cv::Rect &outLeftEyeRegion, cv::Rect &outRightEyeRegion);

        cv::Point2f fineEyeCenter(const cv::Mat eyeROI);

        void computePupilSize(const cv::Mat colorEyeFrame, PupilMeta &pupilMeta);
    };
}

#endif /* Pupilware_hpp */
