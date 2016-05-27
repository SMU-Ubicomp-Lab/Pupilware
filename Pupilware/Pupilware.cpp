//
//  Pupilware.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "Pupilware.hpp"

#include "SignalProcessing/SimpleSignalProcessor.hpp"

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


    void Pupilware::execute( std::shared_ptr<IImageSegmenter>   imgSeg,
                             std::shared_ptr<PWAlgorithm>       algorithm   ) {

        assert(algorithm != nullptr);
        assert(imgSeg != nullptr);

        if (algorithm == nullptr) {
            cout << "[Warning] Algorithm is missing";
            return;
        }

        if (imgSeg == nullptr) {
            cout << "[Warning] Image Segmenter is missing";
            return;
        }


        if (capture.isOpened()) {

            algorithm->init();

            Mat colorFrame;

            while (true) {
                capture >> colorFrame;

                if (!colorFrame.empty()) {

                    executeFrame( colorFrame, imgSeg, algorithm );

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

    void Pupilware::executeFrame(const Mat colorFrame,
                                 std::shared_ptr<IImageSegmenter> imgSeg,
                                 std::shared_ptr<PWAlgorithm> algorithm) {

        assert(!colorFrame.empty());

        std::vector<Mat> frameChannels;
        split(colorFrame, frameChannels);

        //! Use only the red channel.
        //
        Mat frameGray = frameChannels[2];

        cv::Rect faceRect;

        if (!imgSeg->findFace(frameGray, faceRect)) {
            cout << "[Waning] There is no face found this frame." << endl;
            return;
        }


        //! Extract eyes from the frame
        //
        cv::Rect leftEyeRegion;
        cv::Rect rightEyeRegion;
        imgSeg->extractEyes(faceRect, leftEyeRegion, rightEyeRegion);


        //! Find eye center
        //
        Mat grayFace = frameGray(faceRect);
        Point2f leftEyeCenter = imgSeg->fineEyeCenter(grayFace(leftEyeRegion));
        Point2f rightEyeCenter = imgSeg->fineEyeCenter(grayFace(rightEyeRegion));


        //! Compute pupil size
        //
        Mat colorFace = colorFrame(faceRect);

        PupilMeta eyeMeta;

        eyeMeta.setEyeCenter(leftEyeCenter, rightEyeCenter);
        computePupilSize(colorFace(leftEyeRegion),
                         colorFace(rightEyeRegion),
                         eyeMeta, algorithm);


        //! Store data to lists
        //
        leftPupilRadius.push_back( eyeMeta.getLeftPupilRadius() );
        rightPupilRadius.push_back( eyeMeta.getRightPupilRadius() );
        eyeDistance.push_back( cw::calDistance(leftEyeCenter, rightEyeCenter) );
    }


    void Pupilware::computePupilSize(const Mat colorLeftEyeFrame,
                                     const Mat colorRightEyeFrame,
                                     PupilMeta &pupilMeta,
                                     std::shared_ptr<PWAlgorithm> algorithm
    ) {

        assert(algorithm != nullptr);

        algorithm->process(colorLeftEyeFrame, colorRightEyeFrame, pupilMeta);

    }


    void Pupilware::processPupilSignal() {

        std::unique_ptr<BasicSignalProcessor> sp(new BasicSignalProcessor());

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