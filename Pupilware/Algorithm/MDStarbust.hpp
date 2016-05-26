//
//  mdStarbust.hpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#ifndef mdStarbust_hpp
#define mdStarbust_hpp

#include "PWAlgorithm.hpp"

namespace pw {

    class MDStarbust : public PWAlgorithm {
    
    public:
        MDStarbust();
        virtual ~MDStarbust();
        
        virtual void init();
        virtual PWResult process(const cv::Mat colorEyeImage, PupilMeta &pupilMeta);
        virtual void exit();
        
    private:

        const unsigned int MAX_WALKING_STEPS = 20;
        const unsigned int STARBURST_ITERATION = 5;

        const unsigned int MIN_NUM_RAYS = 5;
        const unsigned int MIM_NUM_INLIER_POINTS = 5;

        float degreeOffset;
        float primer;

        void increaseContrast(const cv::Mat &grayEye, const cv::Point &eyeCenter) const;

        void constructRays(std::vector<cv::Point2f> &rays) const;

        void findEdgePoints(cv::Mat grayEye,
                            const cv::Point &startingPoint,
                            const std::vector<cv::Point2f> &rays,
                            std::vector<cv::Point2f> &outEdgePoints,
                            cv::Mat debugColorEye) const;

        bool isValidEllipse(const cv::RotatedRect &theEllipse) const;

        float getCost(int step) const;
    };
}

#endif /* mdStarbust_hpp */
