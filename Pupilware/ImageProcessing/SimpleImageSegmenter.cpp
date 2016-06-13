//
// Created by Chatchai Wangwiwattana on 5/27/16.
//

#include "SimpleImageSegmenter.hpp"

#include "../etc/CWCVUtility.hpp"

using namespace std;
using namespace cv;

namespace pw{

    SimpleImageSegmenter::SimpleImageSegmenter(const std::string &fileFaceCascadePath)
    {
        loadFaceDetectionCascade(fileFaceCascadePath);
    }

    SimpleImageSegmenter::SimpleImageSegmenter( const SimpleImageSegmenter &other ){ }

    SimpleImageSegmenter::~SimpleImageSegmenter(){

    }

    void SimpleImageSegmenter::loadFaceDetectionCascade(const std::string &filePath)
    {
        assert(!filePath.empty());

        if(!faceCascade.load(filePath)){
            cout << "[Error] It cannot read cascade file :( . Make sure you have a valid file." << endl;
        }
    }

    bool SimpleImageSegmenter::findFace(const cv::Mat grayFrame, cv::Rect &outFaceRect) {

        assert(grayFrame.channels() == 1);

        std::vector<cv::Rect> faces;

        // Detect faces
        faceCascade.detectMultiScale(grayFrame,
                                     faces, 1.1, 2,
                                     0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT,
                                     cv::Size(150, 150));


        // Pick only one face for now.
        if (faces.size() > 0) {
            outFaceRect = faces[0];

            return true;

        }
        else {
            cout << "[Info] A face has not found.";
        }

        return false;
    }


    void SimpleImageSegmenter::extractEyes(cv::Rect faceROI,
                                          cv::Rect &outLeftEyeRegion,
                                          cv::Rect &outRightEyeRegion) {

        const float kEyePercentTop = 30.0f;
        const float kEyePercentSide = 13.0f;
        const float kEyePercentHeight = 25.0f;
        const float kEyePercentWidth = 35.0f;

        //-- Find eye regions
        int eye_region_width = static_cast<int>(faceROI.width * (kEyePercentWidth / 100.0f));
        int eye_region_height = static_cast<int>(faceROI.width * (kEyePercentHeight / 100.0f));
        int eye_region_top = static_cast<int>(faceROI.height * (kEyePercentTop / 100.0f));

        cv::Rect leftEyeRegion(static_cast<int>(faceROI.width * (kEyePercentSide / 100.0f)),
                               eye_region_top, eye_region_width, eye_region_height);
        cv::Rect rightEyeRegion(
                static_cast<int>(faceROI.width - eye_region_width - faceROI.width * (kEyePercentSide / 100.0f)),
                eye_region_top, eye_region_width, eye_region_height);


        outLeftEyeRegion = leftEyeRegion;
        outRightEyeRegion = rightEyeRegion;
    }


    cv::Point2f SimpleImageSegmenter::fineEyeCenter(const Mat grayEyeROI) {

        assert(grayEyeROI.channels() == 1);

        if (grayEyeROI.channels() > 1)
            return Point2f();


        Mat blur;
        cv::GaussianBlur(grayEyeROI, blur,Size(3,3), 3);

        std::vector<float>cHist;
        cHist = cw::calProgressiveSum(blur);

        int imgSize = blur.rows*blur.cols;

        int th = 0;
        for (int j = 0; j < cHist.size(); ++j) {
            double ch = cHist[j]/static_cast<double>(imgSize);
            if(ch > 0.005 ){
                th = j;
                break;
            }
        }


        Mat binary;
        cv::threshold(grayEyeROI, binary, th, 255, CV_THRESH_BINARY_INV);

//        cw::erosion(binary, binary, 2, MORPH_ELLIPSE);

        // Calculate center of mass
        Moments m = moments(binary, false);
        cv::Point p(m.m10/m.m00, m.m01/m.m00);

        return p;
    }

}