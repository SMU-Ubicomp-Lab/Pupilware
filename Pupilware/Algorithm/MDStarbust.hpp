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

        virtual PWResult process(const cv::Mat colorLeftEye,
                                 const cv::Mat colorRightEye,
                                 PupilMeta &pupilMeta);

        virtual void exit();
        
    protected:

        const unsigned int MAX_WALKING_STEPS = 20;
        const unsigned int STARBURST_ITERATION = 5;

        const unsigned int MIN_NUM_RAYS = 5;
        const unsigned int MIM_NUM_INLIER_POINTS = 5;

        const float precision = 1000;

        int threshold;
        int rayNumber;
        int degreeOffset;
        int primer;

        float _oldLeftRadius;
        float _oldRightRadius;


        void increaseContrast(const cv::Mat &grayEye, const cv::Point &eyeCenter) const;

        void createRays(std::vector<cv::Point2f> &rays) const;

        void findEdgePoints(cv::Mat grayEye,
                            const cv::Point &startingPoint,
                            const std::vector<cv::Point2f> &rays,
                            std::vector<cv::Point2f> &outEdgePoints,
                            cv::Mat debugColorEye) const;

        bool isValidEllipse(const cv::RotatedRect &theEllipse) const;

        virtual float getCost(int step) const;

        float findPupilSize(const cv::Mat &colorEyeFrame,
                            cv::Point eyeCenter,
                            const char* name) const;
    };
}

#endif /* mdStarbust_hpp */
