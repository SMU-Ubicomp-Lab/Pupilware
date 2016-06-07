//
// Created by Chatchai Wangwiwattana on 6/6/16.
//

#include "MDStarbustNeo.hpp"

namespace pw{

    MDStarbustNeo::MDStarbustNeo( const std::string& name ):
            MDStarbust(name),
            sigma(10000){

        threshold = 0;
    }


    MDStarbustNeo::~MDStarbustNeo(){}


    void MDStarbustNeo::init(){
        MDStarbust::init();

        window->addTrackbar( "sigma", &sigma, 40 * precision);

    }


    float MDStarbustNeo::getCost(int step) const{

        int ksize = 41;

        cv::Mat gaussianKernel = cv::getGaussianKernel(ksize, sigma/precision);


        double min;
        double max;
        cv::minMaxIdx(gaussianKernel, &min, &max);
        cv::Mat adjMap;
        cv::convertScaleAbs(gaussianKernel, adjMap, 255 / max);
        cv::imshow("Gaussian Kernel", adjMap);


        const double scale = primer / precision / max;
//        std::cout << "step " << step << " cost " << *gaussianKernel.ptr<double>(step+20) * scale << std::endl;

        return *gaussianKernel.ptr<double>(step+20) * scale;
    }

}