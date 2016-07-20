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

    //-------- Other fun videos you can pick -----------------------------------
//    const string videoFilePath = dataPath + "videos/ID265489/xpDigitalSpanTask_Digits7_Iter2.wmv"; // bad from webcam
//    const string videoFilePath = dataPath + "videos/ID265493/xpDigitalSpanTask_Digits5_Iter2.wmv"; // brown eyes from webcam
//    const string videoFilePath = dataPath + "videos/ID265502/ExpDigitalSpanTask_Digits5_Iter4.wmv"; // reflection eyes from webcam

    const string videoFilePath = dataPath + "videos/ID265517/ID517Digits7Iter1.mp4"; // good from phone
//    const string videoFilePath = dataPath + "videos/ID265513/Id265513_digit7_iter2.mp4"; // noisy low contrast
//    const string videoFilePath = dataPath + "videos/ID265512/Id265512_digit5_iter3.mp4"; // dark eye, left center mess up, algorithm not working
//    const string videoFilePath = dataPath + "videos/ID265515/ID515Digits7Iter1.mp4"; // black eye

//------------------------------------------------------------------------------

//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit7_iter1.mp4";
//    const string videoFilePath = dataPath + "videos/ID265516/ID516Digits7Iter1.mp4"; // BIG pupil!
//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit7_iter1.mp4"; // Eye center messup when he looks down, (using snake get better result)

//------------------------------------------------------------------------------

//    const string videoFilePath = dataPath + "videos/ID265484/xpMULTIPLICATION_DifficultyMedium_Iter4.wmv"; // good from webcam
    const string faceCascadePath = dataPath + "haarcascade_frontalface_alt.xml";

    const string gazeDataPath = dataPath + "/videos/ID265513/ID265513_ExpDigitalSpanTask_Digits7_Iter2_GazeData.txt";

    // If you want to pre cache the video put true (longer load time, but more control)
    // If not, put fault (good for a large video, and quick experiment)
    std::shared_ptr<Pupilware> pupilware = Pupilware::Create(false);

    pupilware->loadVideo( videoFilePath );


//    pupilware->addPupilSizeAlgorithm(std::make_shared<MDStarbust>("MDStarburst"));
    pupilware->addPupilSizeAlgorithm(std::make_shared<MDStarbustNeo>("MDStarbustNeo"));
    pupilware->addPupilSizeAlgorithm(std::make_shared<MaximumCircleFit>("MaximumCircleFit"));
//    pupilware->addPupilSizeAlgorithm(std::make_shared<ALOfflineFile>("Gaze", gazeDataPath, 852));

    pupilware->execute(std::make_shared<SimpleImageSegmenter>(faceCascadePath) );

    return 0;
}




