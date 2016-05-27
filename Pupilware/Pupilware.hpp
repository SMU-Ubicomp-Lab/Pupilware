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
#include "ImageProcessing/IImageProcessor.hpp"

namespace pw {

    class Pupilware {

    public:
        Pupilware();

        ~Pupilware();

        void loadVideo(const std::string &videoFilePath);

        void setAlgorithm(std::shared_ptr<PWAlgorithm> algorithm);

        void setImageProcessor(std::shared_ptr<IImageProcessor> imgProcessor);

        void execute();

    private:
        cv::VideoCapture capture;

        std::shared_ptr<PWAlgorithm> algorithm;

        std::shared_ptr<IImageProcessor> imgProcessor;

        void executeFrame(const cv::Mat colorFrame);
        void computePupilSize(const cv::Mat colorEyeFrame, PupilMeta &pupilMeta);
    };
}

#endif /* Pupilware_hpp */
