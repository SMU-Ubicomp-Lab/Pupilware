//
//  AlgorithmBase.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "PWAlgorithm.hpp"
#include "../etc/CVWindow.hpp"

namespace pw {

    PWAlgorithm::PWAlgorithm():
    window("Algorithm Debug Window"){

    }

    PWAlgorithm::~PWAlgorithm() { }

    void PWAlgorithm::init() {
        
    }


    void PWAlgorithm::exit() {
        
    }


    void PWAlgorithm::createTrackbar(int *variable, const std::string &label, int max) {

        assert(!label.empty());
        assert(max > 0);
        assert(variable != nullptr);

        window.addTrackbar(label, variable, max);
    }

}