//
// Created by Chatchai Wangwiwattana on 5/26/16.
//

#include "CWUIHelper.hpp"

#include "GraphUtils.h"

namespace cw {
    void showGraph(const char *name, const std::vector<float> &dataSrc, int delayInMilliSec, cv::Scalar color) {
        
        setCustomGraphColor(static_cast<int>(color[0]),
                            static_cast<int>(color[1]),
                            static_cast<int>(color[3]));
        
        showFloatGraph(name,
                       dataSrc.data(),
                       static_cast<int>(dataSrc.size()),
                       delayInMilliSec);
    }

    int showImage(const char *name, const cv::Mat img, int delayInMilliSec) {
        
        cv::namedWindow(name, CV_WINDOW_AUTOSIZE);
        cv::imshow(name, img);
        return cv::waitKey(delayInMilliSec);
        
    }
}