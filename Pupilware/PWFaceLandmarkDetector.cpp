//
//  PWFaceLandmarkDetector.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 9/29/16.
//  Copyright © 2016 SMU Ubicomp Lab. All rights reserved.
//

#include "PWFaceLandmarkDetector.hpp"

#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <sstream>

using namespace dlib;

namespace pw{

//    dlib::frontal_face_detector detector;

    PWFaceLandmarkDetector::PWFaceLandmarkDetector(){
        currentFrame = 0;
    }

    void PWFaceLandmarkDetector::loadLandmarkFile(const std::string& landmarkFilePath)
    {

        this->landmarkFilePath = landmarkFilePath;
        dlib::deserialize(landmarkFilePath) >> sp;

        std::cout << "Landmark has loaded" << std::endl;

//        detector = dlib::get_frontal_face_detector();

    }


    void PWFaceLandmarkDetector::searchLandMark( const cv::Mat& frameBGR,std::vector<cv::Point>& outLandmarkPoints, cv::Rect faceLoc )
    {

        // Convert to dlib image
        dlib::array2d<dlib::bgr_pixel> dlibimg;
        dlib::assign_image(dlibimg, dlib::cv_image<dlib::bgr_pixel>(frameBGR));


        dlib::rectangle oneFaceRect(faceLoc.x, faceLoc.y, faceLoc.x+faceLoc.width, faceLoc.y+faceLoc.height);

        // detect all landmarks
        dlib::full_object_detection shape = sp(dlibimg, oneFaceRect);

        dlib::point left_loc_near;
        dlib::point left_loc_far;

        dlib::point right_loc_near;
        dlib::point right_loc_far;

        // and draw them into the image (samplebuffer)
        for (unsigned long k = 0; k < shape.num_parts(); k++) {

            dlib::point p = shape.part(k);
            // Eye points
            if(k==39){

                left_loc_near = p;
                draw_solid_circle(dlibimg, p, 3, dlib::rgb_pixel(0, 255, 255));
            }else if(k==36){

                left_loc_far = p;
                draw_solid_circle(dlibimg, p, 3, dlib::rgb_pixel(0, 255, 255));
            }else if(k==42){

                right_loc_near = p;
                draw_solid_circle(dlibimg, p, 3, dlib::rgb_pixel(0, 255, 255));
            }else if(k==45){

                right_loc_far = p;
                draw_solid_circle(dlibimg, p, 3, dlib::rgb_pixel(0, 255, 255));
            }
            else
            {
                draw_solid_circle(dlibimg, p, 3, dlib::rgb_pixel(0, 0, 255));
            }


            outLandmarkPoints.push_back(cv::Point(p.x(), p.y()));
        }

        // Cal eye distance
        dlib::point left_loc( (left_loc_near.x() + left_loc_far.x()) / 2,
                              (left_loc_near.y() + left_loc_far.y()) / 2);

        dlib::point right_loc((right_loc_near.x() + right_loc_far.x()) / 2,
                              (right_loc_near.y() + right_loc_far.y()) / 2);

        float fTerm = (left_loc.x()-right_loc.x());
        float lTerm = (left_loc.y()-right_loc.y());
        float dist = sqrt( (fTerm*fTerm) + (lTerm*lTerm) );

        const float std_distance = 120.0f;

        const float scaleFactor = std_distance/dist;

        std::cout << dist << ", " << scaleFactor << std::endl;

        std::vector<cv::Point> roiPointsL;
        std::vector<cv::Point> roiPointsR;

        for (unsigned long k = 0; k < outLandmarkPoints.size(); ++k)
        {
            if(k>=36 && k <= 41)
            {
                roiPointsL.push_back(outLandmarkPoints[k]);
            }
            else if(k>=42 && k <= 47)
            {
                roiPointsR.push_back(outLandmarkPoints[k]);
            }
        }



        const cv::Point* ppt[2] = { roiPointsL.data(),roiPointsR.data() };
        int npt[] = { static_cast<int>(roiPointsL.size()), static_cast<int>(roiPointsR.size()) };

        cv::Mat src = frameBGR;
        cv::resize(frameBGR, src, cv::Size(frameBGR.cols*scaleFactor, frameBGR.rows*scaleFactor));


        cv::Rect eyeRectL = cv::Rect( (left_loc.x()*scaleFactor)-32, (left_loc.y()*scaleFactor)-32, 64,64);
        cv::Rect eyeRectR = cv::Rect( (right_loc.x()*scaleFactor)-32, (right_loc.y()*scaleFactor)-32, 64,64);


//        cv::Mat maskL = cv::Mat::zeros(src.rows, src.cols, CV_8U);
//        cv::fillPoly(maskL, ppt, npt, 1, cv::Scalar(255));
//
//        cv::Mat maskR = cv::Mat::zeros(src.rows, src.cols, CV_8U);
//        cv::fillPoly(maskR, ppt, npt, 2, cv::Scalar(255));
//
//        cv::Mat mTmp;
//        src.copyTo(mTmp, maskL);
//
//        cv::Mat mTmp2;
//        src.copyTo(mTmp2, maskR);
//
//        cv::Rect eyeRectL = cv::boundingRect(roiPointsL);
//        cv::Rect eyeRectR = cv::boundingRect(roiPointsR);
//
//        cv::imshow("leftMark", mTmp(eyeRectL));
//        cv::imshow("rightMark", mTmp2(eyeRectR));
//        cv::waitKey(1);


//        // convert back to OpenCV-Mat
//        out = dlib::toMat(dlibimg).clone();

        std::stringstream ss;
        ss << documentPath << "/L_" << currentFrame <<  ".png";
        cv::imwrite(ss.str(), src(eyeRectL));

        ss.str("");
        ss << documentPath << "/R_" << currentFrame <<  ".png";
        cv::imwrite(ss.str(), src(eyeRectR));


//        std::stringstream ss;
//        ss << documentPath << "/L_" << currentFrame <<  ".png";
//        cv::imwrite(ss.str(), mTmp(eyeRectL));
//
//        ss.str("");
//        ss << documentPath << "/R_" << currentFrame <<  ".png";
//        cv::imwrite(ss.str(), mTmp2(eyeRectR));

    }
}