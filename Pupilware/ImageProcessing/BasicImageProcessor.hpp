//
// Created by Chatchai Wangwiwattana on 5/27/16.
//

#ifndef PUPILWARE_BASICIMAGEPROCESSOR_HPP
#define PUPILWARE_BASICIMAGEPROCESSOR_HPP

#include "IImageProcessor.hpp"

namespace pw{

    class BasicImageProcessor: public IImageProcessor {


    private:
        cv::CascadeClassifier faceCascade;


    public:
        BasicImageProcessor( const std::string &fileFaceCascadePath );
        BasicImageProcessor( const BasicImageProcessor &other );
        ~BasicImageProcessor();

        bool            findFace(const cv::Mat grayFrame,
                                 cv::Rect &outFaceRect);

        void            extractEyes(cv::Rect faceROI,
                                    cv::Rect &outLeftEyeRegion,
                                    cv::Rect &outRightEyeRegion);

        cv::Point2f     fineEyeCenter(const cv::Mat grayEyeROI);

        void            loadFaceDetectionCascade(const std::string &filePath);

    };

}


#endif //PUPILWARE_BASICIMAGEPROCESSOR_HPP
