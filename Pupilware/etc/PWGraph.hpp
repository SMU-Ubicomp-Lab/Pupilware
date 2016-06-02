//
// Created by Chatchai Wangwiwattana on 6/2/16.
//

#ifndef PUPILWARE_PWGRAPH_HPP
#define PUPILWARE_PWGRAPH_HPP

#include <opencv2/opencv.hpp>

namespace pw{

    class PWGraph {
    private:
        IplImage* canvas;
        std::string name;

    public:
        PWGraph(const char* title);
        PWGraph(const PWGraph& other);
        ~PWGraph();

        void drawGraph(const char *name, const std::vector<float> &dataSrc, cv::Scalar color);
        void show() const;
    };

}


#endif //PUPILWARE_PWGRAPH_HPP
