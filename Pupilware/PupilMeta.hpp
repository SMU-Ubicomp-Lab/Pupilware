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

    class PupilMeta {
    private:
        float       leftRadius;
        float       rightRadius;
        cv::Point   leftEyeCenter;
        cv::Point   rightEyeCenter;

        unsigned int frameNumber;

    public:
        PupilMeta();
        PupilMeta(const PupilMeta& other);
        virtual ~PupilMeta();

        unsigned int getFrameNumber() const;
        void         setFrameNumber(unsigned int frameNumber);

        float       getLeftPupilRadius() const;
        void        setLeftRadius(float radius);

        float       getRightPupilRadius() const;
        void        setRightRadius(float radius);

        cv::Point   getLeftEyeCenter() const;
        cv::Point   getRightEyeCenter() const;
        void        setEyeCenter(cv::Point leftEyeCenter,
                                 cv::Point rightEyeCenter);

    };
}

#endif /* PupilMeta_hpp */
