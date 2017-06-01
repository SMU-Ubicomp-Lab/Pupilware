//
//  PWFaceLandmarkDetector.hpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 9/29/16.
//  Copyright © 2016 SMU Ubicomp Lab. All rights reserved.
//

#ifndef PWFaceLandmarkDetector_hpp
#define PWFaceLandmarkDetector_hpp

#include <stdio.h>
#include <string>

#include <dlib/image_processing.h>
#include <dlib/image_io.h>

#include <opencv2/opencv.hpp>


namespace pw{

    class PWFaceLandmarkDetector{

    public:
        PWFaceLandmarkDetector();
        void loadLandmarkFile(const std::string& landmarkFilePath);
        void searchLandMark( const cv::Mat& frameBGR, std::vector<cv::Point>& outLandmarkPoints,
                             cv::Rect faceLoc, cv::Mat& leftEye, cv::Mat& rightEye );

        std::string documentPath;
        unsigned int currentFrame;

    private:
        std::string landmarkFilePath;
        dlib::shape_predictor sp;
    };

}

#endif /* PWFaceLandmarkDetector_hpp */
