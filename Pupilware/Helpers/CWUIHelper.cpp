//
// Created by Chatchai Wangwiwattana on 5/26/16.
//

#include "CWUIHelper.hpp"

#include "simpleGraph/GraphUtils.h"
#include "CVWindow.hpp"

using namespace cv;

namespace cw {


    void showGraph(const std::string& name,
                   const std::vector<float> &dataSrc,
                   int delayInMilliSec,
                   cv::Scalar color) {


        setCustomGraphColor(static_cast<int>(color[0]),
                            static_cast<int>(color[1]),
                            static_cast<int>(color[2]));
        
        showFloatGraph(name.c_str(),
                       dataSrc.data(),
                       static_cast<int>(dataSrc.size()),
                       delayInMilliSec);
    }

    int showImage(const std::string& name, const cv::Mat img, int delayInMilliSec) {
        
        cv::namedWindow(name, CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO);
        cv::imshow(name, img);
        return cv::waitKey(delayInMilliSec);
        
    }

    int showHist( const std::string& name, const cv::Mat& img, int delayInMilliSec){
        assert( img.channels() == 1 );
        assert( !img.empty() );

        /// Establish the number of bins
        int histSize = 256;

        /// Set the ranges ( for B,G,R) )
        float range[] = { 1, 256 } ;
        const float* histRange = { range };

        bool uniform = true; bool accumulate = false;

        cv::Mat hist;

        /// Compute the histograms:
        calcHist( &img, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );

        // Draw the histograms for B, G and R
        int hist_w = 256; int hist_h = 200;
        int bin_w = cvRound( (double) hist_w/histSize );

        cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );

        /// Normalize the result to [ 0, histImage.rows ]
        normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

        /// Draw for each channel
        for( int i = 1; i < histSize; i++ )
        {
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
                  Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
                  Scalar( 0, 255, 255), 1, 8, 0  );
        }

        return showImage(name, histImage, delayInMilliSec);
    }

    int showHistRGB( const std::string& name, const cv::Mat& img, int delayInMilliSec){
        assert( img.channels() == 3 );
        assert( !img.empty() );

        /// Separate the image in 3 places ( B, G and R )
        std::vector<Mat> bgr_planes;
        split( img, bgr_planes );

        /// Establish the number of bins
        int histSize = 256;

        /// Set the ranges ( for B,G,R) )
        float range[] = { 0, 256 } ;
        const float* histRange = { range };

        bool uniform = true; bool accumulate = false;

        Mat b_hist, g_hist, r_hist;

        /// Compute the histograms:
        calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
        calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
        calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

        // Draw the histograms for B, G and R
        int hist_w = 256; int hist_h = 200;
        int bin_w = cvRound( (double) hist_w/histSize );

        Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

        /// Normalize the result to [ 0, histImage.rows ]
        normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

        /// Draw for each channel
        for( int i = 1; i < histSize; i++ )
        {
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                  Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                  Scalar( 255, 0, 0), 1, 8, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                  Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                  Scalar( 0, 255, 0), 1, 8, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                  Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                  Scalar( 0, 0, 255), 1, 8, 0  );
        }

        return showImage(name, histImage, delayInMilliSec);
    }

    std::shared_ptr<pw::CVWindow> createWindow( const std::string& winName){
        return std::shared_ptr<pw::CVWindow>(new pw::CVWindow(winName));
    }

    //----------------------------------------------------------------------------
    //  Simple OpenCV Warper
    //----------------------------------------------------------------------------

    void namedWindow( const std::string& winName, int flag ){

        cv::namedWindow(winName, flag);
    }


    void createTrackbar( const std::string& barName,
                         const std::string& windowName,
                         int& value, int max,
                         cv::TrackbarCallback callback ,
                         void* userData  ){

        cv::createTrackbar( barName, windowName, &value, max, callback, userData);
    }


    void imshow(const std::string& winName, cv::InputArray mat){

        cv::imshow(winName, mat);
    }


    int waitKey(int delay){
        return cv::waitKey(delay);
    }

}