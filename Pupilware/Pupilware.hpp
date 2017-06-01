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

#include "Algorithm/IPupilAlgorithm.hpp"
#include "ImageProcessing/IImageSegmenter.hpp"


namespace pw {

    class CVWindow;

    class Pupilware {

    public:

        /*!
         * load a video file.
         * */
        virtual bool loadVideo(const std::string &videoFilePath)=0;


        /*!
         * load a facial landmark file.
         * */
        virtual bool loadFacialLandmarkModel(const std::string &filePath) =0;


        /*!
         * Execute Pupilware pipeline.
         * */
        virtual void execute( std::shared_ptr<IImageSegmenter> imgProcessor )=0;


        /*!
         * Execute add pupilware algorithm
         * */
        virtual void addPupilSizeAlgorithm( std::shared_ptr<IPupilAlgorithm> algorithm)=0;

        /*!
         * Create the object.
         * */
        static std::shared_ptr<Pupilware> Create(bool isPreCacheVideo);


        /**
         *  Hacking just for exporting. TODO: need refactoring. :P
         * */
        virtual void setEyeOutputPath(std::string path)=0;


    };
}

#endif /* Pupilware_hpp */
