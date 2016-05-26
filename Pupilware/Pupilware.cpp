//
//  Pupilware.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "Pupilware.hpp"

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/ml.h>

#include "etc/CWCVUtility.hpp"

#include "constants.h"

using namespace std;
using namespace cv;

namespace pw {

    const cv::String face_cascade_name = "/Users/redeian/Documents/data/haarcascade_frontalface_alt.xml";

    Pupilware::Pupilware() {
        cv::VideoCapture capture = cv::VideoCapture();
        face_cascade.load(face_cascade_name);
    }


    Pupilware::~Pupilware() {
        if (capture.isOpened()) {
            capture.release();
        }
    }


    void Pupilware::loadVideo(const std::string &videoFilePath) {
        assert(!videoFilePath.empty());

        if (videoFilePath.empty()) {
            cout << "The video name is empty. Please check your path name." << endl;
        }


        if (!capture.open(videoFilePath)) {
            cout << "Cannot read the video. Please check path name." << endl;
        }


    }


    void Pupilware::setAlgorithm(pw::PWAlgorithm *algorithm) {
        assert(algorithm != nullptr);

        this->algorithm = algorithm;
    }


    void Pupilware::execute() {
        assert(algorithm != nullptr);
        if (algorithm == nullptr) {
            cout << "Algorithm is missing";
            return;
        }


        if (capture.isOpened()) {

            algorithm->init();

            Mat frame;

            while (true) {
                capture >> frame;

                if (!frame.empty()) {


                    std::vector<Mat> frameChannels;
                    split(frame, frameChannels);


                    //! Use only the red channel.
                    Mat frameGray = frameChannels[2];

                    cv::Rect faceRect;

                    if (!findFace(frameGray, faceRect)) {
                        cout << "There is no face found this frame." << endl;
                        continue;
                    }


                    //! Extract eyes from the frame
                    cv::Rect leftEyeRegion;
                    cv::Rect rightEyeRegion;
                    extractEyes(faceRect, leftEyeRegion, rightEyeRegion);


                    //! Find eye center
                    Mat grayFace = frameGray(faceRect);
                    Point2f leftEyeCenter = fineEyeCenter(grayFace(leftEyeRegion));
                    Point2f rightEyeCenter = fineEyeCenter(grayFace(rightEyeRegion));


                    //! Compute pupil size
                    Mat colorFace = frame(faceRect);

                    PupilMeta leftEyeMeta;
                    leftEyeMeta.setEyeCenter(leftEyeCenter);
                    leftEyeMeta.setEyeType(PW_LEFT_EYE);
                    computePupilSize(colorFace(leftEyeRegion), leftEyeMeta);

                    PupilMeta rightEyeMeta;
                    rightEyeMeta.setEyeCenter(rightEyeCenter);
                    rightEyeMeta.setEyeType(PW_RIGHT_EYE);
                    computePupilSize(colorFace(rightEyeRegion), rightEyeMeta);


                }
                else {

                    algorithm->exit();

                    cout << " --(!) No captured frame -- Break!";
                    break;
                }

            }

        }
        else {
            cout << "the video has not yet loaded." << endl;
        }
    }

    bool Pupilware::findFace(const cv::Mat grayFrame, cv::Rect &outFaceRect) {
        assert(grayFrame.channels() == 1);

        std::vector<cv::Rect> faces;

        // Detect faces
        face_cascade.detectMultiScale(grayFrame,
                                      faces, 1.1, 2,
                                      0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT,
                                      cv::Size(150, 150));


        // Pick only one face for now.
        if (faces.size() > 0) {
            outFaceRect = faces[0];

            return true;

        }
        else {
            cout << "a face not found.";
        }

        return false;
    }


    void Pupilware::extractEyes(cv::Rect faceROI, cv::Rect &outLeftEyeRegion, cv::Rect &outRightEyeRegion) {

        const float kEyePercentTop = 25.0f;
        const float kEyePercentSide = 13.0f;
        const float kEyePercentHeight = 30.0f;
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


    Point2f Pupilware::fineEyeCenter(const Mat grayEyeROI) {

        assert(grayEyeROI.channels() == 1);

        if (grayEyeROI.channels() > 1)
            return Point2f();

        Mat eq;
        cv::equalizeHist(grayEyeROI, eq);

        Mat binary;
        cv::threshold(eq, binary, 10, 255, CV_THRESH_BINARY_INV);

        cw::erosion(binary, binary, 2, MORPH_ELLIPSE);

        Mat difference = binary;

        float sumx = 0, sumy = 0;
        float num_pixel = 0;
        for (int x = 0; x < difference.cols; x++) {
            for (int y = 0; y < difference.rows; y++) {
                int val = *difference.ptr<uchar>(y, x);
                if (val >= 50) {
                    sumx += x;
                    sumy += y;
                    num_pixel++;
                }
            }
        }

        if (sumx < 3 && sumy < 3) return Point2f(0, 0);

        cv::Point p(sumx / num_pixel, sumy / num_pixel);

//TODO: Test which one is better. (p or p1)
//        Moments m = moments(difference, false);
//        cv::Point p1(m.m10/m.m00, m.m01/m.m00);

        return p;
    }


    void Pupilware::computePupilSize(const Mat colorEyeFrame, PupilMeta &pupilMeta) {

        assert(algorithm != nullptr);

        algorithm->process(colorEyeFrame, pupilMeta);

    }

}