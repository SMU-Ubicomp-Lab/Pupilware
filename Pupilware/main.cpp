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

//    std::cerr << cv::getBuildInformation();

    const string dataPath = "/Users/redeian/Documents/data/";

    const string videoFilePath = dataPath + "videos/ID265517/ID517Digits7Iter1.mp4"; // good from phone

    const string faceCascadePath = dataPath + "haarcascade_frontalface_alt.xml";

    const string gazeDataPath = dataPath + "/videos/ID265517/ID265517_ExpDigitalSpanTask_Digits7_Iter1_GazeData.txt";

    // If you want to pre cache the video put true (longer load time, but more control)
    // If not, put fault (good for a large video, and quick experiment)
    std::shared_ptr<Pupilware> pupilware = Pupilware::Create(true);

    pupilware->loadVideo( videoFilePath );

    pupilware->addPupilSizeAlgorithm(std::make_shared<MDStarbustNeo>("MDStarbustNeo"));
    pupilware->addPupilSizeAlgorithm(std::make_shared<ALOfflineFile>("Gaze", gazeDataPath, 852));

    pupilware->execute(std::make_shared<SimpleImageSegmenter>(faceCascadePath) );

    return 0;
}




