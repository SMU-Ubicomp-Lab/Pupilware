//
// Created by Chatchai Wangwiwattana on 5/27/16.
//

#ifndef PUPILWARE_SIGNALPROCESSINGHELPER_HPP
#define PUPILWARE_SIGNALPROCESSINGHELPER_HPP


#include <vector>

namespace cw{

    typedef float (*filterFunc) ( std::vector<float>&v);

    void medfilt( std::vector<float> &input,
                 std::vector<float> &output,
                 int windowSize);

    void movingAverage( std::vector<float> &input,
                       std::vector<float> &output,
                       int windowSize);

    void trimMeanFilt( std::vector<float> &input,
                      std::vector<float> &output,
                      int windowSize);

    void filterSignal( std::vector<float> &input,
                      std::vector<float> &output,
                      int windowSize,
                      filterFunc filtFunc );

}



#endif //PUPILWARE_SIGNALPROCESSINGHELPER_HPP
