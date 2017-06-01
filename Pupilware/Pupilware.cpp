//
//  Pupilware.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "Pupilware.hpp"

#include "preHeader.hpp"

#include "SignalProcessing/BasicSignalProcessor.hpp"
#include "Helpers/PWGraph.hpp"
#include "Algorithm/PWDataModel.hpp"
#include "SignalProcessing/SignalProcessingHelper.hpp"
#include "PWCSVExporter.hpp"
#include "PWFaceLandmarkDetector.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include <sstream>
#include <fstream>



using namespace std;
using namespace cv;

namespace pw {

    class PupilwareImpl: public Pupilware {
    public:

        cv::VideoCapture capture;


        //TODO: Make these to circular buffers. In case we have a very long video, or streaming
        std::vector<float> eyeDistance;

        std::map<std::shared_ptr<IPupilAlgorithm>,
                 std::shared_ptr<PWDataModel> > algorithms;


        cv::Mat currentFrame;                       // Store ref to a current active frame image.


        std::vector<cv::Mat> videoFrames;           // Store collection of all video frames.
                                                    // It's only used when PreCacheVideo mode = true.

        int currentFrameNumber;                     // Keeping track of frame that is processing

        int isPlaying;                              // If the video is automatic advance frame.

        bool isPreCacheVideo;                       // Whether or not pre-cache video

        std::string videoPath;                      // Store a video path
        PWCSVExporter faceExporter;                 // Help export face meta

        std::shared_ptr<CVWindow> mainWindow;       // Main window to control frames
        std::shared_ptr<PWGraph> pupilSizeGraph;         // Maind display graph

        PWFaceLandmarkDetector landmark;


//------------------------------------------------------------------------------------------------------------------
//      Methods
//------------------------------------------------------------------------------------------------------------------


        virtual void setEyeOutputPath(std::string path){
            landmark.documentPath = path;
        }

        PupilwareImpl(bool isPreCacheVideo):
                currentFrameNumber(0),
                isPlaying(1),
                mainWindow(new CVWindow("MainWindow")),
                isPreCacheVideo(isPreCacheVideo)
        {

            cv::VideoCapture capture = cv::VideoCapture();

            mainWindow->addTrackbar("play", &isPlaying, 1);
            mainWindow->moveWindow(0,0);


            landmark.loadLandmarkFile("/Users/redeian/Documents/projects/Pupilware/Pupilware/shape_predictor_68_face_landmarks.dat");

        }

        PupilwareImpl(const PupilwareImpl &other){}

        ~PupilwareImpl(){

            if (capture.isOpened()) {
                capture.release();
            }

        }

//------------------------------------------------------------------------------------------------------------------

        /*!
         * load a video file.
         * */
        void loadVideo(const std::string &videoFilePath){

            REQUIRES(!videoFilePath.empty(), "The video path must not be empty.");

            if (videoFilePath.empty()) {
                cout << "[Error] The video name is empty. Please check your path name." << endl;
            }


            if (!capture.open(videoFilePath)) {
                cout << "[Error] Cannot read the video. Please check path name." << endl;
            }

            videoPath = videoFilePath;
            faceExporter.open(videoFilePath+"_face.csv");


        }



//------------------------------------------------------------------------------------------------------------------

        void addPupilSizeAlgorithm( std::shared_ptr<IPupilAlgorithm> algorithm){

            REQUIRES(algorithm != nullptr, "Algorithm must not be null.");
            REQUIRES(algorithms.size() < 5, "Only 5 algorithms allows."); // only support maximum to 5 for now.

            std::shared_ptr<PWDataModel> x(new PWDataModel());
            algorithms[algorithm] = x;

        }

//------------------------------------------------------------------------------------------------------------------
        /*!
         * Execute Pupilware pipeline.
         * */
        void execute( std::shared_ptr<IImageSegmenter> imgSeg ){

            REQUIRES(algorithms.size() > 0, "There is no algorithm in the system, please add some.");
            REQUIRES(imgSeg != nullptr, "Image Segmenter must not be null.");

            if (algorithms.size() <= 0) {
                cout << "[Warning] Algorithm is missing. Please add algorithm first";
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

                initAlgorithms();

                while (true) {

                    advanceFrame();

                    Mat colorFrame = getFrame();

                    double secondPFrame = 0.0;

                    if (!colorFrame.empty()) {

                        auto faceMeta = extractFace(colorFrame, imgSeg);

                        //if no face, store zeros.
                        if(faceMeta.getFaceRect().width == 0) {

                            for (auto it : algorithms) {
                                auto algorithm = it.first;
                                auto storage = algorithms[algorithm];
                                storage->setPupilSizeAt(currentFrameNumber, PWPupilSize());

                            }
                        }
                        else{

                            for (auto it : algorithms) {
                                auto algorithm = it.first;

                                auto result = extractPupil(colorFrame, faceMeta, algorithm);

                                //! Store data to lists
                                //
                                auto storage = algorithms[algorithm];
                                storage->setPupilSizeAt( currentFrameNumber, result );

                            }

                        }


                        faceExporter << faceMeta;

                    } else{
                        cerr << "[Info] the frame is empty. The system is terminated." << std::endl;
                        break;
                    }

                    updateUI(colorFrame, secondPFrame);

                    updateGraphs();

                }

                exitAlgorithms();

                processPupilSignal();

                saveDataToFile();

                faceExporter.close();

            }
            else {
                cout << "[Warning] the video has not yet loaded." << endl;
            }

        }

//------------------------------------------------------------------------------------------------------------------

        void exitAlgorithms() const {
            for (auto it : algorithms) {
                    auto algorithm = it.first;
                    algorithm->exit();
                }

        }
//------------------------------------------------------------------------------------------------------------------

        void initAlgorithms() const {
            for (auto it : algorithms) {
                    auto algorithm = it.first;
                    algorithm->init();
                }
        }

//------------------------------------------------------------------------------------------------------------------

        Mat getFrame() const {

            return currentFrame;

        }

//------------------------------------------------------------------------------------------------------------------

        void updateUI(const Mat &colorFrame, double secondPFrame) const {

            Mat debugMat = colorFrame.clone();

//            cv::rectangle(debugMat, cv::Point(0,450), cv::Point(debugMat.cols, debugMat.rows-400), cv::Scalar(255,255,255),-1 );
//            cv::rectangle(debugMat, cv::Point(0,0), cv::Point(debugMat.cols, 300), cv::Scalar(255,255,255),-1 );

            ostringstream convert;
            convert.str("");
            convert <<"Frame Number:"<< static_cast<float>(currentFrameNumber);

            putText(debugMat, convert.str() , Point(50,100),FONT_HERSHEY_SIMPLEX, 2, Scalar(200,90,0),5);
            int key = mainWindow->update(debugMat);

            if(key == 's'){
                saveDataToFile();
                std::cout << "[info] Data is saved." << std::endl;
            }

            // Make the Trackbar value to reflex the actual current frame number.
            mainWindow->setTrackbarValue("frame", currentFrameNumber);

        }

//------------------------------------------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------------------------------------------

        const Scalar colors[5] = {
                Scalar(255, 0, 0),
                Scalar(0, 0, 255),
                Scalar(0, 255, 0),
                Scalar(255, 0, 255),
                Scalar(255, 255, 0)
        };

        void updateGraphs() {

//            const float kMinValue = 0.03f;
//            const float kMaxValue = 0.1f;
//            const float kMinValue = 68.0f;
//            const float kMaxValue = 79.0f;
//            const float kMinValue = 58.0f;
//            const float kMaxValue = 64.0f;
//            const float kMinValue = 72.0f;
//            const float kMaxValue = 78.0f;
            //big
            const float kMinValue = 0.0f;
            const float kMaxValue = 0.0f;
            pupilSizeGraph = std::make_shared<PWGraph>("Original(red) Neo(blue) right pupil size");

            int i =0;
            for(auto it: algorithms) {
                auto storage = it.second;

                pupilSizeGraph->drawGraph("right", storage->getRightPupilSizes(), colors[i], kMinValue, kMaxValue, 0, 250);
                pupilSizeGraph->drawGraph("left", storage->getLeftPupilSizes(), colors[i], kMinValue, kMaxValue, 0, 250);

//                std::vector<float> smoothRight;
//                cw::fastMedfilt(storage->getRightPupilSizes(), smoothRight, 31);
//                pupilSizeGraph->drawGraph("right", smoothRight, colors[i], 0, 30, 0, 250);

                i++;
            }

            pupilSizeGraph->show();

        }

//------------------------------------------------------------------------------------------------------------------

        void initPupilSignalBuffer() {

            if(isPreCacheVideo)
            {
                eyeDistance.resize(videoFrames.size());

                for( auto it: algorithms){
                    auto storage = it.second;
                    storage->resize(videoFrames.size());

                }

            } else{

                // We don't know what the number of frames are.
                // So we init with 0, so that there is something
                // for graph module to work with.
                eyeDistance.resize(1);

            }

        }

//------------------------------------------------------------------------------------------------------------------

        void initUI() {

            if(isPreCacheVideo){
                mainWindow->addTrackbar("frame", &currentFrameNumber, videoFrames.size() - 1);
            }

        }

//------------------------------------------------------------------------------------------------------------------

        PWFaceMeta extractFace(const cv::Mat colorFrame,
                                 std::shared_ptr<IImageSegmenter> imgSeg ) {

            REQUIRES(imgSeg != nullptr, "Image Segmenter must not be null.");
            REQUIRES(!colorFrame.empty(), "The source must not be empty.");

            std::vector<Mat> frameChannels;
            split(colorFrame, frameChannels);

            Mat frameGray = frameChannels[2];   // Use only the red channel.

            cv::Rect faceRect;

            if (!imgSeg->findFace(frameGray, faceRect)) {
                cout << "[Waning] There is no face found this frame." << endl;
                return PWFaceMeta();
            }

            std::vector<cv::Point> landmarkPoints;
            Mat leftEye;
            Mat rightEye;
            landmark.currentFrame = currentFrameNumber;
            landmark.searchLandMark(colorFrame, landmarkPoints, faceRect, leftEye, rightEye);

//            cw::showImage("leftEye",leftEye);
//            cw::showImage("rightEye",rightEye);

            //! Extract eyes from the frame
            cv::Rect leftEyeRegion;
            cv::Rect rightEyeRegion;
            imgSeg->extractEyes(faceRect, leftEyeRegion, rightEyeRegion);


            //! Find eye center
            Mat grayFace = frameGray(faceRect);
            Point2f leftEyeCenter = imgSeg->fineEyeCenter(grayFace(leftEyeRegion));
            Point2f rightEyeCenter = imgSeg->fineEyeCenter(grayFace(rightEyeRegion));

            //! Convert it to face coordinate
            leftEyeRegion.x += faceRect.x;
            leftEyeRegion.y += faceRect.y;
            rightEyeRegion.x += faceRect.x;
            rightEyeRegion.y += faceRect.y;
            leftEyeCenter.x += leftEyeRegion.x;
            leftEyeCenter.y += leftEyeRegion.y;
            rightEyeCenter.x += rightEyeRegion.x;
            rightEyeCenter.y += rightEyeRegion.y;

            //! Compute pupil size
            Mat colorFace = colorFrame(faceRect);

            //! Compute Eye distance in pixel
            float eyeDist = cw::calDistance(leftEyeCenter, rightEyeCenter);

            //! Prepare PupilMeta object
            PWFaceMeta eyeMeta;
            eyeMeta.setLeftEyeCenter(leftEyeCenter);
            eyeMeta.setRightEyeCenter(rightEyeCenter);
            eyeMeta.setLeftEyeClosed(false);
            eyeMeta.setRightEyeClosed(false);
            eyeMeta.setFrameNumber(currentFrameNumber);
            eyeMeta.setEyeDistancePx(eyeDist);
            eyeMeta.setFaceRect(faceRect);
            eyeMeta.setLeftEyeRect(leftEyeRegion);
            eyeMeta.setRightEyeRect(rightEyeRegion);

            eyeMeta.leftEye = leftEye;
            eyeMeta.rightEye = rightEye;

            return eyeMeta;
        }
        /*!
         * Execute Pupilware pipeline only one given frame
         * */
        PWPupilSize extractPupil(const cv::Mat colorFrame,
                          const PWFaceMeta& faceMeta,
                          std::shared_ptr<IPupilAlgorithm> algorithm ){

            REQUIRES(algorithm != nullptr, "Finding Pupil size algorithm must not be null.");
            REQUIRES(!colorFrame.empty(), "The source must not be empty.");


            PWPupilSize result = computePupilSize( colorFrame, faceMeta, algorithm );

            return result;

        }

//------------------------------------------------------------------------------------------------------------------

        /*!
         * Compute pupil size with PWAlgorithm object
         * */
        PWPupilSize computePupilSize( const Mat& src, const PWFaceMeta& meta,
                                      std::shared_ptr<IPupilAlgorithm> algorithm ){

            REQUIRES(algorithm != nullptr, "Pupil Size Algorithm must not be null.");

            return algorithm->process(src, meta);
        }

//------------------------------------------------------------------------------------------------------------------

        /*!
         * Process pupil size data at the end
         * */
        void processPupilSignal(){

            //TODO: Tempolary Disabled for now because we do not need it yet.

            std::cerr << "[INFO] Processing signal is not fully implementated." << std::endl;

//            std::unique_ptr<BasicSignalProcessor> sp(new BasicSignalProcessor());
//
//            std::vector<float> result;
//
//            for( auto it: algorithms){
//                auto storage = it.second;
//                sp->process( storage->getLeftPupilSizes(),
//                             storage->getRightPupilSizes(),
//                             eyeDistance,
//                             result);
//
//            }

//            cw::showGraph("after signal processing", result, 0);

        }

//------------------------------------------------------------------------------------------------------------------

        void saveDataToFile() const{

            for(auto it: algorithms) {
                auto algorithm = it.first;
                auto storage = it.second;

                pw::PWCSVExporter::toCSV( *storage, videoPath +"_"+algorithm->getName()+".csv");

            }
        }

//------------------------------------------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------------------------------------------

    std::shared_ptr<Pupilware> Pupilware::Create(bool isPreCacheVideo){
        return std::make_shared<PupilwareImpl>(isPreCacheVideo);
    }
}