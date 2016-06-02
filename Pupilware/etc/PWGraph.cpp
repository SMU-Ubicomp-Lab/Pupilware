//
// Created by Chatchai Wangwiwattana on 6/2/16.
//

#include "PWGraph.hpp"
#include "GraphUtils.h"
namespace pw
{
    PWGraph::PWGraph(const char *name):
            canvas(nullptr),
            name(name)
    {

    }

    PWGraph::PWGraph(const PWGraph& other){}

    PWGraph::~PWGraph() {
        if(canvas)
            cvReleaseImage(&canvas);
    }

    void PWGraph::drawGraph(const char *name, const std::vector<float> &dataSrc, cv::Scalar color){
        setCustomGraphColor(static_cast<int>(color[0]),
                            static_cast<int>(color[1]),
                            static_cast<int>(color[2]));

        canvas = drawFloatGraph(dataSrc.data(), static_cast<int>(dataSrc.size()), canvas, 0.0,0.0,0,0 );
    }

    void PWGraph::show() const{
        cvNamedWindow( name.c_str() );
        cvShowImage( name.c_str(), canvas );

        cvWaitKey( 10 );		// Note that cvWaitKey() is required for the OpenCV window to show!

    }
}