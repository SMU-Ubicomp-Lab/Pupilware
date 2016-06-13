//
//  cvUtility.cpp
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "CWCVUtility.hpp"

#include <cstdlib>
#include <time.h>


using namespace cv;

namespace cw {
    
    /**  @function Erosion  */
    void erosion( const Mat src, Mat& dst, int erosionSize, int erosionType ) {

        Mat element = getStructuringElement(erosionType,
                                            cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1),
                                            cv::Point(erosionSize, erosionSize));

//        Opening: MORPH_OPEN : 2
//        Closing: MORPH_CLOSE: 3
//        Gradient: MORPH_GRADIENT: 4
//        Top Hat: MORPH_TOPHAT: 5
//        Black Hat: MORPH_BLACKHAT: 6

        cv::morphologyEx(src, dst, 0, element);

    }


    void openOperation( const Mat src, Mat& dst, int size, int type ) {

        Mat element = getStructuringElement(type,
                                            cv::Size(2 * size + 1, 2 * size + 1),
                                            cv::Point(size, size));

//        Opening: MORPH_OPEN : 2
//        Closing: MORPH_CLOSE: 3
//        Gradient: MORPH_GRADIENT: 4
//        Top Hat: MORPH_TOPHAT: 5
//        Black Hat: MORPH_BLACKHAT: 6

        cv::morphologyEx(src, dst, 2, element);

    }

    void closeOperation( const Mat src, Mat& dst, int size, int type ) {

        Mat element = getStructuringElement(type,
                                            cv::Size(2 * size + 1, 2 * size + 1),
                                            cv::Point(size, size));
        cv::morphologyEx(src, dst, 3, element);

    }

    std::vector<unsigned int> calHistogram( const Mat srcGrayImage ) {

        assert( srcGrayImage.channels() == 1);

        std::vector<unsigned int> hist(256);

        for ( int posY=0; posY<srcGrayImage.rows; ++posY) {
            for ( int posX=0; posX<srcGrayImage.cols; ++posX) {
                const uchar *pix=srcGrayImage.ptr<uchar>(posY, posX);
                hist[*pix]++;
            }
        }


        return hist;
    }

    std::vector<float> calProgressiveSum( const std::vector<unsigned int>& histogram ) {

        std::vector<float> CH(256);
        // Cumulative histogram
        CH[0]=histogram[0];

        for (int i=1; i<256;++i) {
            CH[i]=(CH[i-1]+histogram[i]);
        }

        return CH;
    }


    std::vector<float> calProgressiveSum( const Mat srcGrayImage ){

        auto hist = calHistogram(srcGrayImage);
        return calProgressiveSum( hist);

    }

}