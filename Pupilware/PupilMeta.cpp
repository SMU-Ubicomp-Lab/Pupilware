//
//  PupilMeta.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "PupilMeta.hpp"

namespace pw {

    PupilMeta::PupilMeta(){}
    PupilMeta::PupilMeta(const PupilMeta& other){}
    PupilMeta::~PupilMeta(){}


    float PupilMeta::getLeftPupilRadius() const {
        return leftRadius;
    }


    void PupilMeta::setLeftRadius(float radius) {
        if (radius > 0) {
            this->leftRadius = radius;
        }
        else {
            this->leftRadius = 0;
        }

    }

    float PupilMeta::getRightPupilRadius() const {
        return rightRadius;
    }

    void PupilMeta::setRightRadius(float radius){
        if (radius > 0) {
            this->rightRadius = radius;
        }
        else {
            this->rightRadius = 0;
        }

    }

    cv::Point PupilMeta::getLeftEyeCenter() const {
        return leftEyeCenter;
    }

    cv::Point PupilMeta::getRightEyeCenter() const {
        return rightEyeCenter;
    }

    void PupilMeta::setEyeCenter(cv::Point leftEyeCenter,
                                 cv::Point rightEyeCenter) {
        this->leftEyeCenter = leftEyeCenter;
        this->rightEyeCenter = rightEyeCenter;
    }

    unsigned int PupilMeta::getFrameNumber() const{
        return frameNumber;
    }


    void PupilMeta::setFrameNumber(unsigned int frameNumber){
        this->frameNumber = frameNumber;
    }

}