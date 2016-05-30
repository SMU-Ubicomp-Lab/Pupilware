//
// Created by Chatchai Wangwiwattana on 5/30/16.
//

#include "CVWindow.hpp"


namespace pw{

    CVWindow::CVWindow(const std::string& winName):
    winName(winName){
        cv::namedWindow(winName, CV_WINDOW_NORMAL | CV_GUI_NORMAL);
    }

    CVWindow::CVWindow(const CVWindow &other) {

    }

    CVWindow::~CVWindow() {

        if(!winName.empty())
            cv::destroyWindow( this->winName );

    }

    void CVWindow::addTrackbar(const std::string &label, int *value, int max) {
        cv::createTrackbar(label, this->winName, value, max);
    }

    void CVWindow::moveWindow(int x, int y){

        cv::moveWindow(winName, x, y);

    }

    void CVWindow::update(cv::Mat mat ) {

        cv::imshow(winName, mat);
    }

}


