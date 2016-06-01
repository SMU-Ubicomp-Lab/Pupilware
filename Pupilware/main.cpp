#include <opencv2/objdetect/objdetect.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

// Include Pupilware Algorithm here -----
#include "Pupilware.hpp"
#include "Algorithm/MDStarbust.hpp"
#include "Algorithm/DummyAlgo.hpp"

#include "ImageProcessing/SimpleImageSegmenter.hpp"
//--------------------------------------

using namespace cv;
using namespace std;
using namespace pw;


void processPupilSignal();

//--------------------------------------

/**
 * @function main
 */
int main(int argc, const char **argv) {

    const string videoFilePath = "/Users/redeian/Documents/data/ID265484/xpMULTIPLICATION_DifficultyMedium_Iter4.wmv";
    const string faceCascadePath = "/Users/redeian/Documents/data/haarcascade_frontalface_alt.xml";


    Pupilware *pupilware = new Pupilware();

    pupilware->loadVideo( videoFilePath );

    pupilware->execute(std::shared_ptr<SimpleImageSegmenter>(new SimpleImageSegmenter(faceCascadePath)),
                       std::shared_ptr<MDStarbust>(new MDStarbust()) );

//    pupilware->execute(std::shared_ptr<SimpleImageSegmenter>(new SimpleImageSegmenter(faceCascadePath)),
//                       std::shared_ptr<DummyAlgo>(new DummyAlgo()) );

    return 0;
}




