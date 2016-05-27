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
        Pupilware(const Pupilware &other);
        ~Pupilware();


        /*!
         * load a video file.
         * */
        void loadVideo(const std::string &videoFilePath);



        /*!
         * Assigned pupil size finding module
         * */
        void setAlgorithm(std::shared_ptr<PWAlgorithm> algorithm);



        /*!
         * Assigned an image processing module
         * */
        void setImageProcessor(std::shared_ptr<IImageProcessor> imgProcessor);



        /*!
         * Execute Pupilware pipeline.
         * */
        void execute();



    private:

        cv::VideoCapture                    capture;
        std::shared_ptr<PWAlgorithm>        algorithm;
        std::shared_ptr<IImageProcessor>    imgProcessor;


        //TODO: Make these to circular buffers.

        std::vector<float>                  eyeDistance;
        std::vector<float>                  leftPupilRadius;
        std::vector<float>                  rightPupilRadius;



        /*!
         * Execute Pupilware pipeline only one given frame
         * */
        void executeFrame(const cv::Mat colorFrame);



        /*!
         * Compute pupil size with PWAlgorithm object
         * */
        void computePupilSize( const cv::Mat    colorEyeFrame,
                               PupilMeta        &pupilMeta     );


        /*!
         * Process pupil size data at the end
         * */
        void processPupilSignal();
    };
}

#endif /* Pupilware_hpp */
