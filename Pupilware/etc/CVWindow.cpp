//
// Created by Chatchai Wangwiwattana on 5/30/16.
//

#include "CVWindow.hpp"


namespace pw{

    CVWindow::CVWindow(const std::string& winName):
    winName(winName){
        cv::namedWindow(winName, CV_WINDOW_NORMAL);
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

    int CVWindow::update(cv::Mat mat) {

        cv::imshow(winName, mat);
        return cv::waitKey(1);
    }

    void CVWindow::setTrackbarValue( const std::string& name, int value ) const{
        cv::setTrackbarPos(name, winName, value);
    }

}


