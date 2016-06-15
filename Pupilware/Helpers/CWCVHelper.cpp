//
//  cvUtility.cpp
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "CWCVHelper.hpp"

#include "../preHeader.hpp"


using namespace cv;

namespace cw {
    
    /**  @function Erosion  */
    void erosion( const Mat src, Mat& dst, int erosionSize, int erosionType ) {

        REQUIRES( !src.empty(), "The source must not be empty." );
        REQUIRES( erosionSize > 0, "Size must be more than zero. Now size is " << erosionSize );
        REQUIRES( erosionType >=0 && erosionType < 3, "Type must be between 0-2. Now type is " << erosionType );

        Mat element = getStructuringElement(erosionType,
                                            cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1),
                                            cv::Point(erosionSize, erosionSize));

//        Opening: MORPH_OPEN : 2
//        Closing: MORPH_CLOSE: 3
//        Gradient: MORPH_GRADIENT: 4
//        Top Hat: MORPH_TOPHAT: 5
//        Black Hat: MORPH_BLACKHAT: 6

        cv::morphologyEx(src, dst, 0, element);

        PROMISES( !dst.empty(), "Returned matrix must not be empty." );

    }


    void openOperation( const Mat src, Mat& dst, int size, int type ) {

        REQUIRES( !src.empty(), "The source must not be empty." );
        REQUIRES( size > 0, "Size must be more than zero. Now size is " << size );
        REQUIRES( type >=0 && type < 3, "Type must be between 0-2. Now type is " << type );

        Mat element = getStructuringElement(type,
                                            cv::Size(2 * size + 1, 2 * size + 1),
                                            cv::Point(size, size));

//        Opening: MORPH_OPEN : 2
//        Closing: MORPH_CLOSE: 3
//        Gradient: MORPH_GRADIENT: 4
//        Top Hat: MORPH_TOPHAT: 5
//        Black Hat: MORPH_BLACKHAT: 6

        cv::morphologyEx(src, dst, 2, element);

        PROMISES( !dst.empty(), "Returned matrix must not be empty." );

    }


    void closeOperation( const Mat src, Mat& dst, int size, int type ) {

        REQUIRES( !src.empty(), "The source must not be empty." );
        REQUIRES( size > 0, "Size must be more than zero. Now size is " << size );
        REQUIRES( type >=0 && type < 3, "Type must be between 0-2. Now type is " << type );

        Mat element = getStructuringElement(type,
                                            cv::Size(2 * size + 1, 2 * size + 1),
                                            cv::Point(size, size));
        cv::morphologyEx(src, dst, 3, element);

        PROMISES( !dst.empty(), "The returned matrix must not be empty. Please check." );

    }


    std::vector<unsigned int> calHistogram( const Mat srcGray ) {

        REQUIRES( !srcGray.empty(), "The source must not be empty." );
        REQUIRES( srcGray.channels() == 1, "The source Mat must be one channel." );

        std::vector<unsigned int> hist(256);

        for ( int posY=0; posY<srcGray.rows; ++posY) {
            for ( int posX=0; posX<srcGray.cols; ++posX) {
                const uchar *pix=srcGray.ptr<uchar>(posY, posX);
                hist[*pix]++;
            }
        }


        return hist;
    }

//!
//  Snake Circle
// -----------------------------------------------------------------------------------------------------------------

    std::vector<float> calProgressiveSum( const std::vector<unsigned int>& histogram ) {

        REQUIRES( !histogram.empty(), "The histogram must not be empty." );

        std::vector<float> CH(256);
        // Cumulative histogram
        CH[0]=histogram[0];

        for (int i=1; i<256;++i) {
            CH[i]=(CH[i-1]+histogram[i]);
        }

        return CH;
    }


    std::vector<float> calProgressiveSum( const Mat srcGray ){

        REQUIRES( !srcGray.empty(), "The source must not be empty." );
        REQUIRES( srcGray.channels() == 1, "The source Mat must be one channel." );

        auto hist = calHistogram(srcGray);

        return calProgressiveSum( hist);
    }


//!
//  Conversion
// -----------------------------------------------------------------------------------------------------------------

    void cvtFloatMatToUChar(const Mat src, Mat &dist){

        REQUIRES( !src.empty(), "The source must not be empty." );
        REQUIRES( src.channels() == 1, "The source Mat must be one channel." );

        double min;
        double max;
        cv::minMaxIdx(src, &min, &max);
        cv::convertScaleAbs(src, dist, 255 / max);

        PROMISES( !dist.empty(), "The output is empty. Please check." );
    }


    cv::Point calCenterOfMass( const cv::Mat binaryMat ){

        REQUIRES(!binaryMat.empty(), "The Source mat must not be empty.");
        REQUIRES(binaryMat.channels() == 1, "The source mat must be on channel.");

        Moments m = moments(binaryMat, false);

        return cv::Point(m.m10/m.m00, m.m01/m.m00);
    }
}