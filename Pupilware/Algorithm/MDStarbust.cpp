//
//  mdStarbust.cpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#include "MDStarbust.hpp"
#include "../etc/Ransac.h"

using namespace cv;

namespace pw {
    
    MDStarbust::MDStarbust():
    degreeOffset(25),
    primer(1.0f),
    _oldLeftRadius(0.0f),
    _oldRightRadius(0.0f){

    }
    
    MDStarbust::~MDStarbust()
    {
        
    }
    
    void MDStarbust::init()
    {
        // Init code here
    }
    
    PWResult MDStarbust::process(const cv::Mat colorLeftEye, const cv::Mat colorRightEye, PupilMeta &pupilMeta)
    {
        float leftPupilRadius = max(findPupilSize(colorLeftEye, pupilMeta.getLeftEyeCenter(), "left eye"), _oldLeftRadius);
        float rightPupilRadius = max(findPupilSize(colorRightEye, pupilMeta.getRightEyeCenter(), "right eye"), _oldRightRadius);

        //! Store data for next frame used.
        _oldLeftRadius = leftPupilRadius;
        _oldRightRadius = rightPupilRadius;

        pupilMeta.setLeftRadius(leftPupilRadius);
        pupilMeta.setRightRadius(rightPupilRadius);

        return AL_SUCCESS;
    }

    float MDStarbust::findPupilSize(const Mat &colorEyeFrame,
                                    cv::Point eyeCenter,
                                    const char * name) const {

        vector<Mat> rgbChannels(3);
        split(colorEyeFrame, rgbChannels);

        // Only use a red channel.
        Mat grayEye = rgbChannels[2];

        increaseContrast(grayEye, eyeCenter);

        vector<Point2f>rays;
        constructRays(rays);

        Mat debugColorEye = colorEyeFrame.clone();

        vector<Point2f>edgePoints;
        findEdgePoints(grayEye, eyeCenter, rays, edgePoints, debugColorEye);

        if(edgePoints.size() > MIN_NUM_RAYS)
        {
            const float MAX_ERROR_FROM_EDGE_OF_THE_CIRCLE = 1;
            vector<Point2f> inliers;

            //TODO: Parameterized RANSAC class. Can be done after clean up RANSAC class.
            Ransac r;
            r.ransac_circle_fitting(edgePoints,
                                    static_cast<int>(edgePoints.size()),
                                    edgePoints.size()*0.2f,
                                    0.0f ,
                                    MAX_ERROR_FROM_EDGE_OF_THE_CIRCLE,
                                    edgePoints.size()*0.99f,
                                    inliers);


            //---------------------------------------------------------------------------------
            //! Just assigned the best model to PupilMeta object.
            //---------------------------------------------------------------------------------
            if (inliers.size() > MIM_NUM_INLIER_POINTS)
            {
                RotatedRect myEllipse = fitEllipse( inliers );

                float eyeRadius = 0.0f;

                if(isValidEllipse(myEllipse))
                {
                    //TODO: Use RANSAC Circle radius? How about Ellipse wight?
                    eyeRadius = r.bestModel.GetRadius();
                }
                else
                {
                    //TODO: Make it not ZERO. Use the old frame maybe?
                    eyeRadius = 0.0f;
                }

                //---------------------------------------------------------------------------------
                //! Draw debug image
                //---------------------------------------------------------------------------------
                ellipse( debugColorEye, myEllipse, Scalar(0,50,255) );
                circle( debugColorEye,
                            *r.bestModel.GetCenter(),
                            r.bestModel.GetRadius(),
                            Scalar(255,50,255) );

                cw::showImage(name, debugColorEye, 1);

                return eyeRadius;
            }

        }

        return 0.0f;
    }


    bool MDStarbust::isValidEllipse(const RotatedRect &theEllipse) const {
        return max(theEllipse.size.width, theEllipse.size.height) /
                       min(theEllipse.size.width, theEllipse.size.height) < 1.5;
    }


    void MDStarbust::findEdgePoints(Mat grayEye,
                                    const Point &startingPoint,
                                    const vector<Point2f> &rays,
                                    vector<Point2f> &outEdgePoints, Mat debugColorEye) const {


        Point seedPoint = Point(startingPoint.x, startingPoint.y);

        for( int iter = 0; iter < STARBURST_ITERATION; iter++ )
        {

            uchar *seed_intensity = grayEye.ptr<uchar>(startingPoint.y, startingPoint.x);

            for(auto r = rays.begin(); r != rays.end(); r++)
            {
                Point walking_point = seedPoint;
                int walking_intensity = 0;

                for( int i=0; i < MAX_WALKING_STEPS; i++ )
                {
                    Point nextPoint;
                    nextPoint.y = seedPoint.y+(i* r->y);
                    nextPoint.x = seedPoint.x+(r->x * i);

                    // Make sure next point is out of bound.
                    nextPoint.x = min(max(nextPoint.x,0),grayEye.cols - 1);
                    nextPoint.y = min(max(nextPoint.y,0),grayEye.rows - 1);

                    uchar nextPointIntensity = *grayEye.ptr<uchar>(nextPoint.y, nextPoint.x);

                    walking_intensity = nextPointIntensity;
                    walking_point = nextPoint;

                    if((walking_intensity - *seed_intensity - getCost(i)) > 25)
                    {
                        outEdgePoints.push_back(nextPoint);
                        break;
                    }
                }

            }

            // Prepare for next iteration
            if( outEdgePoints.size() > 0)
            {
                // Draw points to the debug image.
                for( int i=0; i<outEdgePoints.size(); i++ )
                {
                    *debugColorEye.ptr<Vec3b>(outEdgePoints[i].y, outEdgePoints[i].x) = Vec3b(0,255,0); // green
                }

                /*!
                 * Traditional Starbust algorithm, a new seed point will be
                 * the mean of all points from previous iteration.
                 *
                 * Uncomment it if you want to use this one. ;)
                 **/

                /*
                int sum_x = 0;
                int sum_y = 0;
                for( int i=0; i<outEdgePoints.size(); i++ )
                {
                    sum_x += outEdgePoints[i].x;
                    sum_y += outEdgePoints[i].y;
                }

                int mean_point_x = sum_x / outEdgePoints.size();
                int mean_point_y = sum_y / outEdgePoints.size();

                int r_x = cw::randomRange(0,1);
                int r_y = cw::randomRange(-4,0);

                seedPoint.x = mean_point_x;
                seedPoint.y = mean_point_y;

                seedPoint.x = min(max(mean_point_x, 0),grayEye.cols - 1);
                seedPoint.y = min(max(mean_point_y, 0),grayEye.rows - 1);

                circle( debugColorEye, Point(mean_point_x, mean_point_y), 1, Scalar(0,0,255));
                */


            }
        }

    }

    float MDStarbust::getCost(int step) const {
        return (primer * (MAX_WALKING_STEPS - step));
    }


    void MDStarbust::constructRays(vector<Point2f> &rays) const {
        const int RAY_NUMBER = 15;
        const float step = M_PI/float(RAY_NUMBER);

        float offset_radians = (degreeOffset * M_PI / 180.0f);

        for(float i=-offset_radians; i < M_PI+offset_radians; i+= step )
        {
            rays.push_back( Point2f( cos(i), sin(i)) );
        }
    }


    void MDStarbust::increaseContrast(const Mat &grayEye, const Point &eyeCenter) const {
        Rect pupil_area = Rect(max(eyeCenter.x - 20, 0),
                               max(eyeCenter.y-20,0),
                               min( grayEye.cols - eyeCenter.x ,40),
                               min( grayEye.rows - eyeCenter.y ,40));

        medianBlur(grayEye, grayEye, 3);

        equalizeHist(grayEye(pupil_area),
                     grayEye(pupil_area));
    }


    void MDStarbust::exit()
    {
        // Clean up code here.
    }
}