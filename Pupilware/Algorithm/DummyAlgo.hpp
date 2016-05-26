//
//  mdStarbust.hpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#ifndef DummyAlgo_hpp
#define DummyAlgo_hpp

#include "PWAlgorithm.hpp"

namespace pw {

    class DummyAlgo : public PWAlgorithm {
    
    public:
        DummyAlgo();
        virtual ~DummyAlgo();
        
        virtual void init();
        virtual PWResult process(cv::Mat colorEyeImage, PupilMeta& outPupilMeta);
        virtual void exit();
        
    private:
        // Your private member variables here.
        
    };
}

#endif /* mdStarbust_hpp */
