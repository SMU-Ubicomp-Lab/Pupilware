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
#include "ImageProcessing/BasicImageProcessor.hpp"
#include "SignalProcessing/BasicSignalProcessor.hpp"
#include "constants.h"

#include <iostream>

using namespace std;
using namespace cv;



namespace pw {


    Pupilware::Pupilware() {
        cv::VideoCapture capture = cv::VideoCapture();

    }

    Pupilware::Pupilware(const Pupilware &other) { }

    Pupilware::~Pupilware() {
        if (capture.isOpened()) {
            capture.release();
        }
    }


    void Pupilware::loadVideo(const std::string &videoFilePath) {
        assert(!videoFilePath.empty());

        if (videoFilePath.empty()) {
            cout << "[Error] The video name is empty. Please check your path name." << endl;
        }


        if (!capture.open(videoFilePath)) {
            cout << "[Error] Cannot read the video. Please check path name." << endl;
        }

    }


    void Pupilware::setAlgorithm(std::shared_ptr<PWAlgorithm> algorithm) {
        assert(algorithm != nullptr);

        this->algorithm = algorithm;
    }

    void Pupilware::setImageProcessor(std::shared_ptr<IImageProcessor> imgProcessor){
        this->imgProcessor = imgProcessor;
    }


    void Pupilware::execute() {
        assert(algorithm != nullptr);
        assert(imgProcessor != nullptr);

        if (algorithm == nullptr) {
            cout << "[Warning] Algorithm is missing";
            return;
        }


        if (capture.isOpened()) {

            algorithm->init();

            Mat colorFrame;

            while (true) {
                capture >> colorFrame;

                if (!colorFrame.empty()) {

                    executeFrame(colorFrame);

                }
                else {

                    algorithm->exit();

                    processPupilSignal();

                    cout << " No captured frame -- Break";
                    break;
                }

            }

        }
        else {
            cout << "[Warning] the video has not yet loaded." << endl;
        }
    }

    void Pupilware::executeFrame(const Mat colorFrame){

        assert(!colorFrame.empty());

        std::vector<Mat> frameChannels;
        split(colorFrame, frameChannels);

        //! Use only the red channel.
        //
        Mat frameGray = frameChannels[2];

        cv::Rect faceRect;

        if (!imgProcessor->findFace(frameGray, faceRect)) {
            cout << "[Waning] There is no face found this frame." << endl;
            return;
        }


        //! Extract eyes from the frame
        //
        cv::Rect leftEyeRegion;
        cv::Rect rightEyeRegion;
        imgProcessor->extractEyes(faceRect, leftEyeRegion, rightEyeRegion);


        //! Find eye center
        //
        Mat grayFace = frameGray(faceRect);
        Point2f leftEyeCenter = imgProcessor->fineEyeCenter(grayFace(leftEyeRegion));
        Point2f rightEyeCenter = imgProcessor->fineEyeCenter(grayFace(rightEyeRegion));


        //! Compute pupil size
        //
        Mat colorFace = colorFrame(faceRect);

        PupilMeta leftEyeMeta;
        leftEyeMeta.setEyeCenter(leftEyeCenter);
        leftEyeMeta.setEyeType(PW_LEFT_EYE);
        computePupilSize(colorFace(leftEyeRegion), leftEyeMeta);

        PupilMeta rightEyeMeta;
        rightEyeMeta.setEyeCenter( rightEyeCenter );
        rightEyeMeta.setEyeType( PW_RIGHT_EYE );
        computePupilSize( colorFace( rightEyeRegion ), rightEyeMeta );

        //! Store data to lists
        //
        leftPupilRadius.push_back( leftEyeMeta.getRadius() );
        rightPupilRadius.push_back( rightEyeMeta.getRadius() );
        eyeDistance.push_back( cw::calDistance(leftEyeCenter, rightEyeCenter) );
    }


    void Pupilware::computePupilSize(const Mat colorEyeFrame, PupilMeta &pupilMeta) {

        assert(algorithm != nullptr);

        algorithm->process(colorEyeFrame, pupilMeta);

    }

    void Pupilware::processPupilSignal(){

        std::unique_ptr<BasicSignalProcessor>sp(new BasicSignalProcessor());

        std::vector<float> result;

        sp->process(leftPupilRadius,
                    rightPupilRadius,
                    eyeDistance, result);

        cw::showGraph("left pupil size", leftPupilRadius, 1);
        cw::showGraph("right pupil size", rightPupilRadius, 1);
        cw::showGraph("eye distance", eyeDistance, 1);
        cw::showGraph("after signal processing", result, 0);

    }
}