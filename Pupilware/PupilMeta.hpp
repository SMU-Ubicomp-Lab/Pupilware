//
//  PupilMeta.hpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//
//  Brief: It contains Pupil meta data.
//

#ifndef PupilMeta_hpp
#define PupilMeta_hpp

#include <opencv2/opencv.hpp>

namespace pw {
    enum EyeType {
        PW_LEFT_EYE,
        PW_RIGHT_EYE
    };

    class PupilMeta {
    private:
        float       radius;
        cv::Point   eyeCenter;
        EyeType     eyeType;


    public:
        PupilMeta();
        PupilMeta(const PupilMeta& other);
        virtual ~PupilMeta();


        float       getRadius() const;

        void        setRadius(float radius);

        cv::Point   getEyeCenter() const;

        void        setEyeCenter(cv::Point eyeCenter);

        EyeType     getEyeType() const;

        void        setEyeType(EyeType type);
    };
}

#endif /* PupilMeta_hpp */
