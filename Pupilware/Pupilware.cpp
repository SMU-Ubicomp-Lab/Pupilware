//
//  Pupilware.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "Pupilware.hpp"
#include "preHeader.hpp"

#include "Helpers/PWGraph.hpp"
#include "Algorithm/PWDataModel.hpp"
#include "SignalProcessing/SignalProcessingHelper.hpp"
#include "PWCSVExporter.hpp"


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

        int isPlaying;                              // If the video is playing. I used int instead of bool because I can link with OpenCV UI.

        bool isPreCacheVideo;                       // Whether or not pre-cache video

        bool m_forceStop;                           // force stop execution.

        std::string videoPath;                      // Store a video path

        PWCSVExporter faceExporter;                 // Help export face meta

        std::shared_ptr<CVWindow> mainWindow;       // Main window to control frames
        std::shared_ptr<PWGraph> pupilSizeGraph;    // Main display graph

        // for drawing a graph
        const Scalar colors[5] = {
                Scalar(255, 0, 0),
                Scalar(0, 0, 255),
                Scalar(0, 255, 0),
                Scalar(255, 0, 255),
                Scalar(255, 255, 0)
        };
//------------------------------------------------------------------------------------------------------------------
//      Methods
//------------------------------------------------------------------------------------------------------------------


        PupilwareImpl(bool isPreCacheVideo):
                currentFrameNumber(0),
                isPlaying(1),
                mainWindow(new CVWindow("MainWindow")),
                isPreCacheVideo(isPreCacheVideo),
                m_forceStop(false)
        {

            cv::VideoCapture capture = cv::VideoCapture();

            mainWindow->addTrackbar("play", &isPlaying, 1);
            mainWindow->resize(500, 500);
            mainWindow->moveWindow(0,0);


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
        bool loadVideo(const std::string &videoFilePath){

            REQUIRES(!videoFilePath.empty(), "The video path must not be empty.");

            if (videoFilePath.empty()) {
                cout << "[Error] The video name is empty. Please check your path name." << endl;
                return false;
            }


            if (!capture.open(videoFilePath)) {
                cout << "[Error] Cannot read the video. Please check path name." << endl;
                return false;
            }

            videoPath = videoFilePath;


            return true;

        }


//------------------------------------------------------------------------------------------------------------------

        void addPupilSizeAlgorithm( std::shared_ptr<IPupilAlgorithm> algorithm){

            REQUIRES(algorithm != nullptr, "Algorithm must not be null.");
            REQUIRES(algorithms.size() < 100, "Only 100 algorithms allows."); // only support maximum to 100 for now.

            const size_t _old_algorithm_size = algorithms.size();

            std::shared_ptr<PWDataModel> x(new PWDataModel());
            algorithms[algorithm] = x;

            PROMISES( algorithms.size() == _old_algorithm_size+1, "when adding a new algorithm the size must increase by one." );

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

                // Init submodules
                preCacheVideoFrames();
                initUI();
                initPupilSignalBuffer();
                initAlgorithms();

                // main loop
                while (!m_forceStop) {

                    advanceFrame();
                    Mat colorFrame = getFrame();

                    if (colorFrame.empty()) {
                        std::cout << "The frame is empty. The system is terminated." << std::endl;
                        break;
                    }
                    if(cv::waitKey(1) == static_cast<int>('q'))
                    {
                        std::cout << "The system is terminated by a user" << std::endl;
                        break;
                    }

                    auto faceMeta = extractFace( colorFrame, imgSeg );
                    extractPupilsWithAlgorithms( algorithms, colorFrame, faceMeta );
                    updateUI(colorFrame, 0.0);
                    updateGraphs();

                }

                exitAlgorithms();

            }
            else {
                cout << "[Warning] the video has not yet loaded." << endl;
            }

        }

//------------------------------------------------------------------------------------------------------------------

        void extractPupilsWithAlgorithms( std::map<std::shared_ptr<IPupilAlgorithm>, std::shared_ptr<PWDataModel> > &algorithms,
                                          cv::Mat colorFrame,
                                          const PWFaceMeta& faceMeta ) {
            REQUIRES(algorithms.size() > 0, "There is no algorithm in the system, please add some.");

             for (auto it : algorithms) {

                auto algorithm = it.first; // algorithm for the list
                auto storage = algorithms[algorithm]; // get a storage that associate with the algorithm

                auto result = PWPupilSize();

                if(hasFace( faceMeta )) {
                    result = algorithm->process(colorFrame, faceMeta);
                }

                storage->setPupilSizeAt(currentFrameNumber, result); // update storage

            }
        }

//------------------------------------------------------------------------------------------------------------------

        const bool hasFace( const PWFaceMeta& faceMeta ) const{
            return (faceMeta.getFaceRect().width != 0);
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

        void updateUI(const Mat &colorFrame, double secondPFrame) {

            Mat debugMat = colorFrame.clone();

            ostringstream convert;
            convert.str("");
            convert <<"Frame Number:"<< static_cast<float>(currentFrameNumber);

            putText(debugMat, convert.str() , Point(50,100),FONT_HERSHEY_SIMPLEX, 2, Scalar(200,90,0),5);
            int key = mainWindow->update(debugMat);

            if(key=='q')
            {
                m_forceStop = true;
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

        void updateGraphs() {

            const float kMinValue = 0.0f;
            const float kMaxValue = 0.0f;
            pupilSizeGraph = std::make_shared<PWGraph>("Starburst(red) : 2LevelSnakes(blue)");

            int i =0;
            for(auto it: algorithms) {
                auto storage = it.second;

                pupilSizeGraph->drawGraph("right", storage->getRightPupilSizes(), colors[i], kMinValue, kMaxValue, 0, 250);
                pupilSizeGraph->drawGraph("left", storage->getLeftPupilSizes(), colors[i], kMinValue, kMaxValue, 0, 250);

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

        PWFaceMeta extractFace( const cv::Mat colorFrame,
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

            return eyeMeta;
        }




//------------------------------------------------------------------------------------------------------------------

        void saveToFiles( const std::string &outputPath ) const{

            if (outputPath.empty() == true) {
                std::cout << "output path is empty" << std::endl;
                return;
            }

            for(auto it: algorithms) {
                auto algorithm = it.first;
                auto storage = it.second;

                std::cout << "write results to " << outputPath +"_"+algorithm->getName()+".csv" << std::endl;
                pw::PWCSVExporter::toCSV( *storage, outputPath +"_"+algorithm->getName()+".csv");
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
