//
//  AlgorithmBase.hpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#ifndef AlgorithmBase_hpp
#define AlgorithmBase_hpp

#include <opencv2/opencv.hpp>
#include "../PupilMeta.hpp"

#include "../etc/CWUIHelper.hpp"
#include "../etc/CWCVUtility.hpp"


namespace pw{
    
    enum PWResult{
        AL_SUCCESS = 1,
        AL_ERROR = 0
    };

    class PWAlgorithm {
        
    public:
        PWAlgorithm();

        virtual ~PWAlgorithm();

        
        virtual void init();
        
        virtual PWResult process(const cv::Mat colorLeftEye, const cv::Mat colorRightEye, PupilMeta &pupilMeta) =0;
        
        virtual void exit();


    protected:
        cv::Mat debugImg;
        void createTrackbar(int *variable, const std::string &label, int max = 255);

    private:
        CVWindow window;
        
    };
}


#endif /* AlgorithmBase_hpp */
