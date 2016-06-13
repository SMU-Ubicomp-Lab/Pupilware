//
// Created by Chatchai Wangwiwattana on 5/27/16.
//

#include "SimpleImageSegmenter.hpp"

#include "../etc/CWCVUtility.hpp"
#include "../etc/CWUIHelper.hpp"

using namespace std;
using namespace cv;

namespace pw{

    SimpleImageSegmenter::SimpleImageSegmenter(const std::string &fileFaceCascadePath)
    {
        loadFaceDetectionCascade(fileFaceCascadePath);
    }

    SimpleImageSegmenter::SimpleImageSegmenter( const SimpleImageSegmenter &other ){ }

    SimpleImageSegmenter::~SimpleImageSegmenter(){

    }

    void SimpleImageSegmenter::loadFaceDetectionCascade(const std::string &filePath)
    {
        assert(!filePath.empty());

        if(!faceCascade.load(filePath)){
            cout << "[Error] It cannot read cascade file :( . Make sure you have a valid file." << endl;
        }
    }

    bool SimpleImageSegmenter::findFace(const cv::Mat grayFrame, cv::Rect &outFaceRect) {

        assert(grayFrame.channels() == 1);

        std::vector<cv::Rect> faces;

        // Detect faces
        faceCascade.detectMultiScale(grayFrame,
                                     faces, 1.1, 2,
                                     0 | CV_HAAR_SCALE_IMAGE | CV_HAAR_FIND_BIGGEST_OBJECT,
                                     cv::Size(150, 150));


        // Pick only one face for now.
        if (faces.size() > 0) {
            outFaceRect = faces[0];

            return true;

        }
        else {
            cout << "[Info] A face has not found.";
        }

        return false;
    }


    void SimpleImageSegmenter::extractEyes(cv::Rect faceROI,
                                          cv::Rect &outLeftEyeRegion,
                                          cv::Rect &outRightEyeRegion) {

        const float kEyePercentTop = 30.0f;
        const float kEyePercentSide = 13.0f;
        const float kEyePercentHeight = 25.0f;
        const float kEyePercentWidth = 35.0f;

        //-- Find eye regions
        int eye_region_width = static_cast<int>(faceROI.width * (kEyePercentWidth / 100.0f));
        int eye_region_height = static_cast<int>(faceROI.width * (kEyePercentHeight / 100.0f));
        int eye_region_top = static_cast<int>(faceROI.height * (kEyePercentTop / 100.0f));

        cv::Rect leftEyeRegion(static_cast<int>(faceROI.width * (kEyePercentSide / 100.0f)),
                               eye_region_top, eye_region_width, eye_region_height);
        cv::Rect rightEyeRegion(
                static_cast<int>(faceROI.width - eye_region_width - faceROI.width * (kEyePercentSide / 100.0f)),
                eye_region_top, eye_region_width, eye_region_height);


        outLeftEyeRegion = leftEyeRegion;
        outRightEyeRegion = rightEyeRegion;
    }



    void calRegionEnergy(const cv::Mat srcGray, unsigned int& outSumIntensity,
                           unsigned int& outPixelCount, cv::Point center, int radius){

        cv::Point start( center.x - radius, center.y - radius );
        cv::Point end( center.x + radius, center.y + radius );

        start.x = max(start.x, 0);
        start.y = max(start.y, 0);
        end.x = min(end.x, srcGray.cols);
        end.y = min(end.y, srcGray.rows);

        const int radiusSq = radius*radius;

        unsigned int sumIntensity = 0;
        unsigned int pixelNumber = 0;

        for (int y = start.y; y < end.y; ++y) {
            for (int x = start.x; x < end.x; ++x) {

                if(  ( (y - center.y) * (y - center.y) )
                    +( (x - center.x) * (x - center.x) ) <= radiusSq  ){
                    const uchar* intensity = srcGray.ptr<uchar>(y,x);
                    sumIntensity += *intensity;
                    pixelNumber ++;
                }
            }
        }

        outSumIntensity= sumIntensity;
        outPixelCount = (pixelNumber);

    }

    double calSnake(const cv::Mat srcGray, cv::Point center, int radius){

        assert(srcGray.channels() == 1 );

        const double alpha = 2.0;
        const int outerRadius = radius;
        const int innerRadius = radius * (1.0/sqrt(alpha));

        unsigned int outerIntensity = 0;
        unsigned int outerPixel = 0;

        unsigned int innerIntensity = 0;
        unsigned int innerPixel = 0;

        calRegionEnergy(srcGray, outerIntensity, outerPixel, center, outerRadius);
        calRegionEnergy(srcGray, innerIntensity, innerPixel, center, innerRadius);

        float outerEnergy = (outerIntensity - innerIntensity) / static_cast<double>( outerPixel - innerPixel );
        float innerEnergy = innerIntensity / static_cast<double>(innerPixel);
        float diff = outerEnergy - innerEnergy;

//        std::cout << outerEnergy << " " << innerEnergy <<  " " << diff << std::endl;

        return diff;


    }


    int radius = 10;

    cv::Point2f SimpleImageSegmenter::fineEyeCenter(const Mat grayEyeROI) {

        assert(grayEyeROI.channels() == 1);

        if (grayEyeROI.channels() > 1)
            return Point2f();


        Mat blur;
        cv::GaussianBlur(grayEyeROI, blur,Size(3,3), 3);

        std::vector<float>cHist;
        cHist = cw::calProgressiveSum(blur);

        int imgSize = blur.rows*blur.cols;

        int th = 0;
        for (int j = 0; j < cHist.size(); ++j) {
            double ch = cHist[j]/static_cast<double>(imgSize);
            if(ch > 0.005 ){
                th = j;
                break;
            }
        }


        Mat binary;
        cv::threshold(grayEyeROI, binary, th, 255, CV_THRESH_BINARY_INV);


        // Calculate center of mass
        Moments m = moments(binary, false);
        cv::Point p(m.m10/m.m00, m.m01/m.m00);

        // ---- Test space ----------------------
        // Calculate energy 6 directions.
        // Move toward to the direction that has the lowest engergy.
        {
//            Mat testMat = Mat::zeros(300,300, CV_8UC1);
//            cv::circle(testMat, Point(150,150),10,cv::Scalar(255), -1, cv::FILLED );

            Mat testMat = blur.clone();

            cv::imshow("cir", testMat);
            cv::waitKey(1);

            Mat eMat(testMat.size(), CV_32FC1);

            for (int y = 0; y < eMat.rows; ++y) {
                for (int x = 0; x <eMat.cols; ++x) {
                    double e = calSnake(testMat, Point(x,y), radius);
                    *eMat.ptr<float>(y,x) = e;
                }
            }

            Mat eMatD;
            cw::getImageByMatFloat(eMat, eMatD);
            cw::createTrackbar("radius", "emat", radius, 100);
            cw::showImage("emat",eMatD,0);


        }


        // ---- Test space ----------------------

        return p;
    }



}