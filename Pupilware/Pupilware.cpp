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


    class PupilwareImpl: public Pupilware {
    public:

        cv::VideoCapture                    capture;


        //TODO: Make these to circular buffers. In case we have a very long video, or streaming
        std::vector<float>                  eyeDistance;
        std::vector<float>                  leftPupilRadius;
        std::vector<float>                  rightPupilRadius;


        // Store ref to a current active frame image.
        cv::Mat                             currentFrame;


        // Store collection of all video frames.
        // It's only used when PreCacheVideo mode = true.
        std::vector<cv::Mat>                videoFrames;


        int currentFrameNumber;


        // If the video is automatic advance frame.
        int isPlaying;


        // Whether or not pre-cache video
        bool isPreCacheVideo;


        // Main window to control frames
        std::shared_ptr<CVWindow> mainWindow;


        PupilwareImpl(bool isPreCacheVideo):
                currentFrameNumber(0),
                isPlaying(0),
                mainWindow(new CVWindow("MainWindow")),
                isPreCacheVideo(isPreCacheVideo)
        {

            cv::VideoCapture capture = cv::VideoCapture();
            mainWindow->addTrackbar("play", &isPlaying, 1);


        }

        PupilwareImpl(const PupilwareImpl &other){}

        ~PupilwareImpl(){

            if (capture.isOpened()) {
                capture.release();
            }

        }


        /*!
         * load a video file.
         * */
        void loadVideo(const std::string &videoFilePath){
            assert(!videoFilePath.empty());

            if (videoFilePath.empty()) {
                cout << "[Error] The video name is empty. Please check your path name." << endl;
            }


            if (!capture.open(videoFilePath)) {
                cout << "[Error] Cannot read the video. Please check path name." << endl;
            }

        }


        /*!
         * Execute Pupilware pipeline.
         * */
        void execute( std::shared_ptr<IImageSegmenter> imgSeg,
                      std::shared_ptr<PWAlgorithm> algorithm          ){

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

                preCacheVideoFrames();

                initUI();

                initPupilSignalBuffer();

                algorithm->init();

                while (true) {

                    advanceFrame();

                    Mat colorFrame = getFrame();

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

                    updateUI(colorFrame, secondPFrame);

                    updateGraphs();

                }

                algorithm->exit();

                processPupilSignal();

            }
            else {
                cout << "[Warning] the video has not yet loaded." << endl;
            }

        }

        Mat getFrame() const {

            return currentFrame;

        }


        void updateUI(const Mat &colorFrame, double secondPFrame) const {

            Mat debugMat = colorFrame.clone();

            ostringstream convert;
            convert.str("");
            convert <<"SPF:"<< static_cast<float>(secondPFrame);

            putText(debugMat, convert.str() , Point(50,100),FONT_HERSHEY_SIMPLEX, 2, Scalar(200,90,0),5);
            int key = mainWindow->update(debugMat);

            // Make the Trackbar value to reflex the actual current frame number.
            mainWindow->setTrackbarValue("frame", currentFrameNumber);

        }


        void advanceFrame() {

            if(isPlaying == 1)
            {
                currentFrameNumber++;

                if(isPreCacheVideo){
                    currentFrameNumber = (currentFrameNumber < videoFrames.size()) ? currentFrameNumber : videoFrames.size() - 1;
                    currentFrame = videoFrames[currentFrameNumber];
                }
                else{
                    capture >> currentFrame;
                }

            } else
            {

                if(isPreCacheVideo){
                    currentFrame = videoFrames[currentFrameNumber];
                }else{
                    if(currentFrame.empty()) {
                        capture >> currentFrame;
                    }
                }

            }

        }


        void updateGraphs() const {

            // If you want to plot many graphs at the same plot, use PWGraph instead
            //
            shared_ptr<PWGraph> leftGraph(new PWGraph("Left-(red) Right-(green) pupil size"));
            leftGraph->drawGraph("left", leftPupilRadius, Scalar(255, 0, 0) );
            leftGraph->drawGraph("right", rightPupilRadius, Scalar(0, 255, 255) );
            leftGraph->show();


            // If you want to plot quick graph
            //
            cw::showGraph("eye distance", eyeDistance, 1);
        }


        void initPupilSignalBuffer() {

            if(isPreCacheVideo)
            {
                eyeDistance.resize(videoFrames.size());
                leftPupilRadius.resize(videoFrames.size());
                rightPupilRadius.resize(videoFrames.size());
            } else{

                // We don't know what the number of frames are.
                // So we init with 0, so that there is something
                // for graph module to work with.
                eyeDistance.resize(1);
                leftPupilRadius.resize(1);
                rightPupilRadius.resize(1);
            }

        }


        void initUI() {

            if(isPreCacheVideo)
                mainWindow->addTrackbar("frame", &currentFrameNumber, videoFrames.size() - 1);

        }


        /*!
         * Execute Pupilware pipeline only one given frame
         * */
        void executeFrame(const cv::Mat colorFrame,
                          std::shared_ptr<IImageSegmenter> imgSeg,
                          std::shared_ptr<PWAlgorithm> algorithm ){
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

            if(isPreCacheVideo){
                leftPupilRadius[currentFrameNumber] = ( eyeMeta.getLeftPupilRadius() );
                rightPupilRadius[currentFrameNumber] = ( eyeMeta.getRightPupilRadius() );
                eyeDistance[currentFrameNumber] = ( cw::calDistance(leftEyeCenter, rightEyeCenter) );
            }
            else{
                if(currentFrameNumber >= leftPupilRadius.size() )
                {
                    leftPupilRadius.push_back( eyeMeta.getLeftPupilRadius() );
                    rightPupilRadius.push_back( eyeMeta.getRightPupilRadius() );
                    eyeDistance.push_back( cw::calDistance(leftEyeCenter, rightEyeCenter) );

                } else{

                    leftPupilRadius[currentFrameNumber] = ( eyeMeta.getLeftPupilRadius() );
                    rightPupilRadius[currentFrameNumber] = ( eyeMeta.getRightPupilRadius() );
                    eyeDistance[currentFrameNumber] = ( cw::calDistance(leftEyeCenter, rightEyeCenter) );
                }

            }
        }



        /*!
         * Compute pupil size with PWAlgorithm object
         * */
        void computePupilSize( const cv::Mat colorLeftEyeFrame,
                               const cv::Mat colorRightEyeFrame,
                               PupilMeta &pupilMeta,
                               std::shared_ptr<PWAlgorithm> algorithm ){

            assert(algorithm != nullptr);

            algorithm->process(colorLeftEyeFrame, colorRightEyeFrame, pupilMeta);
        }



        /*!
         * Process pupil size data at the end
         * */
        void processPupilSignal(){

            std::unique_ptr<BasicSignalProcessor> sp(new BasicSignalProcessor());

            std::vector<float> result;

            sp->process(leftPupilRadius,
                        rightPupilRadius,
                        eyeDistance, result);

            cw::showGraph("after signal processing", result, 0);

        }


        void preCacheVideoFrames(){

            if(isPreCacheVideo){
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

        }
    };


    Pupilware* Pupilware::Create(bool isPreCacheVideo){
        return new PupilwareImpl(isPreCacheVideo);
    }
}