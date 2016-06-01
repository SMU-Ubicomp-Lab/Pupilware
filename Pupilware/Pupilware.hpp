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
#include "ImageProcessing/IImageSegmenter.hpp"


namespace pw {

    class CVWindow;

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
         * Execute Pupilware pipeline.
         * */
        void execute( std::shared_ptr<IImageSegmenter> imgProcessor,
                      std::shared_ptr<PWAlgorithm> algorithm          );



    private:

        cv::VideoCapture                    capture;


        //TODO: Make these to circular buffers.

        std::vector<float>                  eyeDistance;
        std::vector<float>                  leftPupilRadius;
        std::vector<float>                  rightPupilRadius;


        std::vector<cv::Mat>                 videoFrames;

        int currentFrame;
        bool isPlaying;

        std::shared_ptr<CVWindow> mainWindow;

        /*!
         * Execute Pupilware pipeline only one given frame
         * */
        void executeFrame(const cv::Mat colorFrame,
                          std::shared_ptr<IImageSegmenter> imgSeg,
                          std::shared_ptr<PWAlgorithm> algorithm );



        /*!
         * Compute pupil size with PWAlgorithm object
         * */
        void computePupilSize( const cv::Mat colorLeftEyeFrame,
                               const cv::Mat colorRightEyeFrame,
                               PupilMeta &pupilMeta,
                               std::shared_ptr<PWAlgorithm> algorithm );



        /*!
         * Process pupil size data at the end
         * */
        void processPupilSignal();

        void preCacheVideoFrames();
    };
}

#endif /* Pupilware_hpp */
