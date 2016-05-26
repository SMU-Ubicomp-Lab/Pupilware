//
// Created by Chatchai Wangwiwattana on 5/26/16.
//

#ifndef PUPILWARE_CWUIHELPER_HPP
#define PUPILWARE_CWUIHELPER_HPP

#include <vector>
#include <opencv2/opencv.hpp>

namespace cw {

    void showGraph(const char *name,
                   const std::vector<float> &dataSrc,
                   int delayInMilliSec = 1,
                   cv::Scalar color = cv::Scalar(0, 0, 0));


    int showImage(const char *name, const cv::Mat img, int delayInMilliSec = 1);


}

#endif //PUPILWARE_CWUIHELPER_HPP
