#include <opencv2/objdetect/objdetect.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

// Include Pupilware Algorithm here -----
#include "Pupilware.hpp"
#include "Algorithm/MDStarbust.hpp"
#include "Algorithm/MDStarbustG.hpp"
#include "Algorithm/MDStarbustNeo.hpp"
#include "Algorithm/MaximumCircleFit.hpp"
#include "Algorithm/DummyAlgo.hpp"
#include "Algorithm/ALOfflineFile.hpp"
#include "Algorithm/PixelCount.hpp"
#include "Algorithm/MDStarbustFuzzy.hpp"

#include "Algorithm/BlinkDetection.hpp"

#include "ImageProcessing/SimpleImageSegmenter.hpp"
#include "SignalProcessing/SignalProcessingHelper.hpp"
//#include "SignalProcessing/sgsmooth.h"

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


//    std::cerr << cv::getBuildInformation();

    const string dataPath = "/Users/redeian/Documents/data/";

    //-------- Other fun videos you can pick -----------------------------------
//    const string videoFilePath = dataPath + "videos/ID265489/xpDigitalSpanTask_Digits7_Iter2.wmv"; // bad from webcam
//    const string videoFilePath = dataPath + "videos/ID265493/xpDigitalSpanTask_Digits5_Iter2.wmv"; // brown eyes from webcam
//    const string videoFilePath = dataPath + "videos/ID265502/ExpDigitalSpanTask_Digits5_Iter4.wmv"; // reflection eyes from webcam

    const string videoFilePath = dataPath + "videos/ID265517/ID517Digits7Iter1.mp4"; // good from phone
//    const string videoFilePath = dataPath + "videos/ID265513/Id265513_digit7_iter2.mp4"; // noisy low contrast
//    const string videoFilePath = dataPath + "videos/ID265512/Id265512_digit5_iter3.mp4"; // dark eye, left center mess up
//    const string videoFilePath = dataPath + "videos/ID265515/ID515Digits7Iter1.mp4"; // black eye
//    const string videoFilePath = dataPath + "videos/ID265516/ID516Digits7Iter1.mp4"; // big eye from phone
//    const string videoFilePath = dataPath + "videos/IDX/idx3.mp4"; // x participant
//------------------------------------------------------------------------------


//    const string videoFilePath = dataPath + "videos/ID265517/ID517Digits6Iter1.mp4"; // good from phone

    const string faceCascadePath = dataPath + "haarcascade_frontalface_alt.xml";

    const string gazeDataPath = dataPath + "/videos/ID265517/ID265517_ExpDigitalSpanTask_Digits6_Iter1_GazeData.txt";

    // If you want to pre cache the video put true (longer load time, but more control)
    // If not, put fault (good for a large video, and quick experiment)
    std::shared_ptr<Pupilware> pupilware = Pupilware::Create(false);

    pupilware->loadVideo( videoFilePath );

    pupilware->addPupilSizeAlgorithm(std::make_shared<PixelCount>("PC"));
    pupilware->addPupilSizeAlgorithm(std::make_shared<MaximumCircleFit>("Max"));
    pupilware->addPupilSizeAlgorithm(std::make_shared<BlinkDetection>("Blink"));
    pupilware->addPupilSizeAlgorithm(std::make_shared<MDStarbustNeo>("MDStarbustNeo"));

//    pupilware->addPupilSizeAlgorithm(std::make_shared<ALOfflineFile>("Gaze", gazeDataPath, 885));
//    pupilware->addPupilSizeAlgorithm(std::make_shared<MDStarbustFuzzy>("Fuzzy"));

    pupilware->execute(std::make_shared<SimpleImageSegmenter>(faceCascadePath) );

    return 0;
}




