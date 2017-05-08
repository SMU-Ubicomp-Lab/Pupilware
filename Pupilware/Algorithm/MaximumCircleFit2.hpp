//
//  MaximumCircleFit2.hpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#ifndef MAXIMUM_CIRCLE_FIT2_HPP
#define MAXIMUM_CIRCLE_FIT2_HPP

#include "IPupilAlgorithm.hpp"

namespace pw {

    class MaximumCircleFit2 : public IPupilAlgorithm {

    public:
        MaximumCircleFit2( const std::string& name );
        MaximumCircleFit2( const std::string& name, int mode );
        MaximumCircleFit2( const MaximumCircleFit2& other)=default;
        MaximumCircleFit2( MaximumCircleFit2&& other)=default;
        MaximumCircleFit2& operator=( const MaximumCircleFit2& other)=default;
        MaximumCircleFit2& operator=( MaximumCircleFit2&& other)=default;
        virtual ~MaximumCircleFit2();

        virtual void init() override ;

        virtual PWPupilSize process( const cv::Mat& src, const PWFaceMeta &meta ) override;

        virtual void exit() override ;
        
        const cv::Mat& getDebugImage() const;

        
        /* Setter and Getter */
        void setMode( int mode );
        
    private:


        // It is used in dynamic thresholding
        int mode;

        double ticks;

        cv::Mat measurement = cv::Mat::zeros(1, 1, CV_32F);


        // Just a window name for debuging
        std::shared_ptr<CVWindow> window;
        
        // Debug Image
        cv::Mat debugImage;


        float findPupilSize(const cv::Mat &colorEyeFrame,
                            cv::Point eyeCenter,
                            cv::Mat &debugImg);

        float estimatePupilSize( float left, float right);

        float calCircularEnergy(const cv::Mat& src,
                                const cv::Point& center,
                                int radius,
                                size_t& outSum,
                                size_t& outCount);
    };
}

#endif /* MAXIMUM_CIRCLE_FIT2_HPP */
