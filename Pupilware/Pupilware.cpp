//
//  Pupilware.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "Pupilware.hpp"

#include "SignalProcessing/SimpleSignalProcessor.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include <sstream>
#include "etc/PWGraph.hpp"

using namespace std;
using namespace cv;


namespace pw {


    Pupilware::Pupilware():
    currentFrame(0),
    isPlaying(0),
    mainWindow(new CVWindow("MainWindow"))
    {

        cv::VideoCapture capture = cv::VideoCapture();
        mainWindow->addTrackbar("play", &isPlaying, 1);


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


        ostringstream convert;

        if (capture.isOpened()) {

            preCacheVideoFrames();

            mainWindow->addTrackbar("frame", &currentFrame, videoFrames.size()-1);
            eyeDistance.resize(videoFrames.size());
            leftPupilRadius.resize(videoFrames.size());
            rightPupilRadius.resize(videoFrames.size());

            algorithm->init();

            Mat colorFrame;

            while (true) {
                colorFrame = videoFrames[currentFrame];

                double secondPFrame = 0.0;

                if (!colorFrame.empty()) {
                    double e1 = cv::getTickCount();
                    executeFrame( colorFrame, imgSeg, algorithm );
                    double e2 = cv::getTickCount();

                    secondPFrame = (e2-e1)/cv::getTickFrequency();
                } else{
                    cerr << "[Error] the frame is empty.";
                    break;
                }

                Mat debugMat = colorFrame.clone();

                convert.str("");
                convert <<"SPF:"<< static_cast<float>(secondPFrame);

                cv::putText(debugMat, convert.str() , cv::Point(50,100),FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(200,90,0),5);
                int key = mainWindow->update(debugMat);
                if( key == 'p')
                    isPlaying = !isPlaying;
                else if( key == 'e')
                    break;


                // If you want to plot many graphs at the same plot, use PWGraph instead
                //
                std::shared_ptr<pw::PWGraph> leftGraph(new pw::PWGraph("Left-(red) Right-(green) pupil size"));
                leftGraph->drawGraph("left", leftPupilRadius, cv::Scalar(255,0,0) );
                leftGraph->drawGraph("right", rightPupilRadius, cv::Scalar(0,255,255) );
                leftGraph->show();


                // If you want to plot quick graph
                //
                cw::showGraph("eye distance", eyeDistance, 1);

                if(isPlaying == 1)
                {
                    currentFrame++;
                    currentFrame = (currentFrame < videoFrames.size()) ? currentFrame:videoFrames.size()-1;

                    mainWindow->setTrackbarValue("frame", currentFrame);
                }

            }

            algorithm->exit();

            processPupilSignal();

        }
        else {
            cout << "[Warning] the video has not yet loaded." << endl;
        }
    }

    void Pupilware::preCacheVideoFrames() {
        Mat colorFrame;

        while (true) {
            capture >> colorFrame;

                if (!colorFrame.empty()) {
                    videoFrames.push_back(colorFrame.clone());
                }
                else {
                    cout << " Finished Load frame. There are " << videoFrames.size() << " frames" << endl;
                    break;
                }

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
        leftPupilRadius[currentFrame] = ( eyeMeta.getLeftPupilRadius() );
        rightPupilRadius[currentFrame] = ( eyeMeta.getRightPupilRadius() );
        eyeDistance[currentFrame] = ( cw::calDistance(leftEyeCenter, rightEyeCenter) );
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

        cw::showGraph("after signal processing", result, 0);

    }
}