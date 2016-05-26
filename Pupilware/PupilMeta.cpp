//
//  PupilMeta.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "PupilMeta.hpp"

namespace pw {

    float PupilMeta::getRadius() const {
        return radius;
    }


    void PupilMeta::setRadius(float radius) {
        if (radius > 0) {
            this->radius = radius;
        }
        else {
            this->radius = 0;
        }

    }


    cv::Point PupilMeta::getEyeCenter() const {
        return eyeCenter;
    }


    void PupilMeta::setEyeCenter(cv::Point eyeCenter) {
        this->eyeCenter = eyeCenter;
    }


    EyeType PupilMeta::getEyeType() const {
        return eyeType;
    }


    void PupilMeta::setEyeType(EyeType type) {
        this->eyeType = type;
    }

}