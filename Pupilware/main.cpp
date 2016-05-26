#include <opencv2/objdetect/objdetect.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

// Include Pupilware Algorithm here -----
#include "Pupilware.hpp"
#include "Algorithm/MDStarbust.hpp"
#include "Algorithm/DummyAlgo.hpp"

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


    Pupilware *pupilware = new Pupilware();
    pupilware->loadVideo(videoFilePath);

    //pupilware->setAlgorithm(new DummyAlgo());

    pupilware->setAlgorithm(new MDStarbust());

    pupilware->execute();

    processPupilSignal();

    return 0;
}


void processPupilSignal() {
    // TODO: Add something man! FASTER!
}



