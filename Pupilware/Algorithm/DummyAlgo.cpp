//
//  mdStarbust.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "DummyAlgo.hpp"

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


    }


    PWResult DummyAlgo::process(cv::Mat colorEyeImage, PupilMeta& outPupilMeta)
    {
        // Processing code here
        outPupilMeta.setRadius(10.0f);

        // push data to test drawing a graph.
        dummyGraphData.push_back(cw::randomRange(0, 1000) / 100.0f);

        // Draw a graph (default 1 millisec delay, and black)
        cw::showGraph("black Graph", dummyGraphData);

        // Draw a red graph with 100 millisec delay.
        cw::showGraph("red graph", dummyGraphData, 100, cv::Scalar(255, 0, 0));


        cw::showImage("frame4", colorEyeImage, 1);
        return PWResult::AL_SUCCESS;
    }


    void DummyAlgo::exit()
    {
        // Clean up code here.
        std::cout << "Clean up Dummy Algorithm." << std::endl;
    }
}