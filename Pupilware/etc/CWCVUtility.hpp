//
//  cvUtility.hpp
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#ifndef cvUtility_hpp
#define cvUtility_hpp

#include <opencv2/opencv.hpp>

#include "GraphUtils.h"

namespace cw {
    
    void erosion( const cv::Mat src, cv::Mat& dst, int erosionSize=2, int erosionType=cv::MORPH_ELLIPSE );

    int randomRange(int from, int to);
}


#endif /* cvUtility_hpp */
