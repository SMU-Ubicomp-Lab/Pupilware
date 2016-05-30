//
//  mdStarbust.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "DummyAlgo.hpp"

#include <opencv/highgui.h>

namespace pw {

    std::vector<float> dummyGraphData;

    DummyAlgo::DummyAlgo(){
        
    }
    
    DummyAlgo::~DummyAlgo()
    {
        
    }

    
    void DummyAlgo::init()
    {
        // Init code here
        std::cout << "Init Dummy Algorithm." << std::endl;


        for (int i = 0; i < 20; ++i) {
            dummyGraphData.push_back(i);
        }

        thWin = std::shared_ptr<CVWindow>(new CVWindow("threshold"));
        thWin->addTrackbar("Threshold", &th, 255);
        thWin->moveWindow(500, 100);

    }


    PWResult DummyAlgo::process(const cv::Mat colorLeftEye, const cv::Mat colorRightEye, PupilMeta &pupilMeta)
    {
        // Processing code here
        pupilMeta.setLeftRadius(10.0f);
        pupilMeta.setRightRadius(20.0f);

        // push data to test drawing a graph.
        dummyGraphData.push_back(cw::randomRange(0, 1000) / 100.0f);

        // Draw a graph (default 1 millisec delay, and black)
        cw::showGraph("black Graph", dummyGraphData);

        // Draw a red graph with 100 millisec delay.
        cw::showGraph("red graph", dummyGraphData, 100, cv::Scalar(255, 0, 0));


        cw::showImage("frame4", colorLeftEye, 1);


        cv::Mat thresholdImg;

        cv::createButton("threshold1",nullptr, nullptr,CV_CHECKBOX,1 );


        // Block frame and keep updating until pressing "return" key
        // 27 esc key
        // 13 return key
        while(cw::waitKey(33) != 13){

            cv::threshold(colorLeftEye, thresholdImg, th, 255, CV_THRESH_BINARY);

            thWin->update(thresholdImg);
        }

        return PWResult::AL_SUCCESS;
    }


    void DummyAlgo::exit()
    {
        cvDestroyAllWindows();

        // Clean up code here.
        std::cout << "Clean up Dummy Algorithm." << std::endl;
    }
}