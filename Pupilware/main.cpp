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

//    const string videoFilePath = dataPath + "videos/ID265517/ID517Digits7Iter1.mp4"; // good from phone
//    const string videoFilePath = dataPath + "videos/ID265513/Id265513_digit7_iter2.mp4"; // noisy low contrast
//    const string videoFilePath = dataPath + "videos/ID265512/Id265512_digit5_iter3.mp4"; // dark eye, left center mess up
//    const string videoFilePath = dataPath + "videos/ID265515/ID515Digits7Iter1.mp4"; // black eye
//    const string videoFilePath = dataPath + "videos/ID265516/ID516Digits7Iter1.mp4"; // big eye from phone
//    const string videoFilePath = dataPath + "videos/IDX/idx3.mp4"; // x participant
//------------------------------------------------------------------------------


//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit5_iter1.mp4";
//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit5_iter2.mp4";
//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit5_iter3.mp4";
//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit5_iter4.mp4";
//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit6_iter1.mp4";
//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit6_iter2.mp4";
//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit6_iter3.mp4";
//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit6_iter4.mp4";
//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit7_iter1.mp4";
//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit7_iter2.mp4";
//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit7_iter3.mp4";
//    const string videoFilePath = dataPath + "videos/ID265510/Id265510_digit7_iter4.mp4";

//12,16,17
    std::string videoPath = "/Users/redeian/Google Drive/ParticipantsMP4Videos/";
    std::vector<std::string> videos;
    videos.push_back(videoPath + "ID515Digits5iter1.mp4");
    videos.push_back(videoPath + "ID515Digits5iter2.mp4");
    videos.push_back(videoPath + "ID515Digits5iter3.mp4");
    videos.push_back(videoPath + "ID515Digits5iter4.mp4");
    videos.push_back(videoPath + "ID515Digits6iter1.mp4");
    videos.push_back(videoPath + "ID515Digits6iter2.mp4");
    videos.push_back(videoPath + "ID515Digits6iter3.mp4");
    videos.push_back(videoPath + "ID515Digits6iter4.mp4");
    videos.push_back(videoPath + "ID515Digits7iter1.mp4");
    videos.push_back(videoPath + "ID515Digits7iter2.mp4");
    videos.push_back(videoPath + "ID515Digits7iter3.mp4");
    videos.push_back(videoPath + "ID515Digits7iter4.mp4");
    videos.push_back(videoPath + "ID515Digits8iter1.mp4");
    videos.push_back(videoPath + "ID515Digits8iter2.mp4");
    videos.push_back(videoPath + "ID515Digits8iter3.mp4");
    videos.push_back(videoPath + "ID515Digits8iter4.mp4");
//    videos.push_back(dataPath + "videos/ID265512/Id265512_digit9_iter1.mp4");
//    videos.push_back(dataPath + "videos/ID265512/Id265512_digit9_iter2.mp4");
//    videos.push_back(dataPath + "videos/ID265512/Id265512_digit9_iter3.mp4");
//    videos.push_back(dataPath + "videos/ID265512/Id265512_digit9_iter4.mp4");

    std::vector<std::string> eyePaths;
    eyePaths.push_back(dataPath + "eyes/id515/t5_1");
    eyePaths.push_back(dataPath + "eyes/id515/t5_2");
    eyePaths.push_back(dataPath + "eyes/id515/t5_3");
    eyePaths.push_back(dataPath + "eyes/id515/t5_4");
    eyePaths.push_back(dataPath + "eyes/id515/t6_1");
    eyePaths.push_back(dataPath + "eyes/id515/t6_2");
    eyePaths.push_back(dataPath + "eyes/id515/t6_3");
    eyePaths.push_back(dataPath + "eyes/id515/t6_4");
    eyePaths.push_back(dataPath + "eyes/id515/t7_1");
    eyePaths.push_back(dataPath + "eyes/id515/t7_2");
    eyePaths.push_back(dataPath + "eyes/id515/t7_3");
    eyePaths.push_back(dataPath + "eyes/id515/t7_4");
    eyePaths.push_back(dataPath + "eyes/id515/t8_1");
    eyePaths.push_back(dataPath + "eyes/id515/t8_2");
    eyePaths.push_back(dataPath + "eyes/id515/t8_3");
    eyePaths.push_back(dataPath + "eyes/id515/t8_4");
//    eyePaths.push_back(dataPath + "eyes/id512/t9_1");
//    eyePaths.push_back(dataPath + "eyes/id512/t9_2");
//    eyePaths.push_back(dataPath + "eyes/id512/t9_3");
//    eyePaths.push_back(dataPath + "eyes/id512/t9_4");

//      const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit5_iter1.mp4";
//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit5_iter2.mp4";
//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit5_iter3.mp4";
//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit5_iter4.mp4";
//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit6_iter1.mp4";
//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit6_iter2.mp4";
//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit6_iter3.mp4";
//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit6_iter4.mp4";
//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit7_iter1.mp4";
//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit7_iter2.mp4";
//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit7_iter3.mp4";
//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit7_iter4.mp4";
//    const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit8_iter1.mp4";
//        const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit8_iter2.mp4";
//        const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit8_iter3.mp4";
//        const string videoFilePath = dataPath + "videos/ID265511/Id265511_digit8_iter4.mp4";
//    const string videoFilePath = "/Users/redeian/Desktop/untitled folder/mark_digit_lux1_dgt6_itr2_face.mp4";

    const string faceCascadePath = dataPath + "haarcascade_frontalface_alt.xml";

    for( int i=0; i<videos.size(); ++i) {


        // If you want to pre cache the video put true (longer load time, but more control)
        // If not, put fault (good for a large video, and quick experiment)
        std::shared_ptr<Pupilware> pupilware = Pupilware::Create(false);

        pupilware->setEyeOutputPath(eyePaths[i]);

        pupilware->loadVideo(videos[i]);

        pupilware->addPupilSizeAlgorithm(std::make_shared<PixelCount>("PC"));
//    pupilware->addPupilSizeAlgorithm(std::make_shared<MaximumCircleFit>("Max"));
//    pupilware->addPupilSizeAlgorithm(std::make_shared<BlinkDetection>("Blink"));
//    pupilware->addPupilSizeAlgorithm(std::make_shared<MDStarbustNeo>("MDStarbustNeo"));
//    pupilware->addPupilSizeAlgorithm(std::make_shared<MDStarbustFuzzy>("Fuzzy"));

        pupilware->execute(std::make_shared<SimpleImageSegmenter>(faceCascadePath));
    }
    return 0;
}




