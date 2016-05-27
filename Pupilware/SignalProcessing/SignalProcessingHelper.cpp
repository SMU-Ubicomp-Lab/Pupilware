//
// Created by Chatchai Wangwiwattana on 5/27/16.
//

#include "SignalProcessingHelper.hpp"

#include <cassert>
#include <numeric>

namespace cw {


    float median( std::vector<float> &v )
    {
        if (v.size() <= 0)
        {
            return 0.0f;
        }

        std::vector<float>cpy;
        cpy.assign(v.begin(), v.end());

        size_t n = v.size() / 2;
        nth_element(cpy.begin(), cpy.begin()+n, cpy.end());
        return cpy[n];
    }


    float trimMean( std::vector<float> &v)
    {
        if (v.size() <= 0)
        {
            return 0.0f;
        }

        sort(v.begin(), v.end());

        int trimSize = static_cast<int>(v.size() * 0.2);

        double sum_of_elems = std::accumulate(v.begin()+trimSize,v.end()-trimSize, 0);
        return sum_of_elems/(v.size() - (trimSize*2));
    }


    float average( std::vector<float> &v)
    {
        if (v.size() <= 0)
        {
            return 0.0f;
        }

        float sum = 0.0f;
        for( const float & item : v)
        {
            sum += item;
        }

        return sum/(float)v.size();
    }

    void medfilt( std::vector<float>& input,
                 std::vector<float>& output, int windowSize)
    {
        filterSignal(input, output, windowSize, median);
    }

    void movingAverage( std::vector<float>& input, std::vector<float>& output, int windowSize)
    {
        filterSignal(input, output, windowSize, average);
    }

    void trimMeanFilt( std::vector<float>& input, std::vector<float>& output, int windowSize)
    {
        filterSignal(input, output, windowSize, trimMean);
    }


    void filterSignal( std::vector<float> &input,
                      std::vector<float> &output,
                      int windowSize, filterFunc filtFunc) {

        assert(input.size() > 0);
        assert(windowSize > 0);

        if (input.size() <= 0) {
            return;
        }

        if (windowSize <= 0) {
            return;
        }

        if (input.size() < windowSize) {
            windowSize = (int) input.size();
        }

        // allocate memory
        output.resize(input.size());

        int midPos = windowSize / 2;

        // fill the first haft of data
        for (size_t i = 0; i < midPos; i++) {
            auto p_start = input.begin();
            auto p_end = input.begin() + i + 1;

            std::vector<float> nWindow;
            nWindow.assign(p_start, p_end);

            float m = filtFunc(nWindow);
            output[i] = m;

        }

        // fill the rest of the data
        for (size_t i = 0; i < input.size() - midPos; i++) {

            auto p_start = input.begin() + i;
            auto p_end = input.begin() + i + windowSize;

            std::vector<float> nWindow;
            nWindow.assign(p_start, p_end);

            float m = filtFunc(nWindow);
            output[midPos + i] = m;
        }

        assert(output.size() > 0);
    }

}