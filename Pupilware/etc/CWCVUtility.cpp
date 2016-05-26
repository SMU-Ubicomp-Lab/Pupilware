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
    void erosion( const Mat src, Mat& dst, int erosionSize, int erosionType )
    {
        
        Mat element = getStructuringElement( erosionType,
                                            cv::Size( 2*erosionSize + 1, 2*erosionSize+1 ),
                                            cv::Point( erosionSize, erosionSize ) );
        
        cv::morphologyEx( src, dst, 0, element );
        
    }


    int randomRange(int from, int to) {
        /* initialize random seed: */
//        srand (time(NULL));

        return rand() % to + from;
    }


}