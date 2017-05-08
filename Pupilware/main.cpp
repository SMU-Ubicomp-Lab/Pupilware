#include <opencv2/objdetect/objdetect.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

// Include Pupilware Algorithm here -----
#include "Pupilware.hpp"
#include "Algorithm/MDStarbust.hpp"
#include "Algorithm/MDStarbustG.hpp"
#include "Algorithm/MDStarbustNeo.hpp"
#include "Algorithm/MaximumCircleFit.hpp"
#include "Algorithm/MaximumCircleFit2.hpp"
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

    const string dataPath = "/Volumes/MarkBits/pupilwareWebcam/";

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



    std::string p_id = "p18";

//    std::string videoPath = "/Users/redeian/Documents/data/videos/ID265" + p_id + "/";

//    std::string videoPath = "/Users/redeian/Documents/data/videos/ID265500/";

    std::string videoPath = "/Volumes/Seagate Backup Plus Drive/pupilwareData/" + p_id + "/";

    std::vector<std::string> videos;


    videos.push_back(videoPath + p_id + "_digit_lux1_dgt6_itr1_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux1_dgt6_itr2_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux1_dgt6_itr3_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux1_dgt6_itr4_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux1_dgt7_itr1_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux1_dgt7_itr2_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux1_dgt7_itr3_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux1_dgt7_itr4_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux1_dgt8_itr1_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux1_dgt8_itr2_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux1_dgt8_itr3_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux1_dgt8_itr4_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux2_dgt6_itr1_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux2_dgt6_itr2_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux2_dgt6_itr3_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux2_dgt6_itr4_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux2_dgt7_itr1_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux2_dgt7_itr2_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux2_dgt7_itr3_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux2_dgt7_itr4_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux2_dgt8_itr1_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux2_dgt8_itr2_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux2_dgt8_itr3_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux2_dgt8_itr4_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux3_dgt6_itr1_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux3_dgt6_itr2_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux3_dgt6_itr3_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux3_dgt6_itr4_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux3_dgt7_itr1_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux3_dgt7_itr2_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux3_dgt7_itr3_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux3_dgt7_itr4_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux3_dgt8_itr1_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux3_dgt8_itr2_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux3_dgt8_itr3_face.mp4");
    videos.push_back(videoPath + p_id + "_digit_lux3_dgt8_itr4_face.mp4");



    std::vector<std::string> eyePaths;


    eyePaths.push_back(dataPath + p_id + "/lum1/t6_1");
    eyePaths.push_back(dataPath + p_id + "/lum1/t6_2");
    eyePaths.push_back(dataPath + p_id + "/lum1/t6_3");
    eyePaths.push_back(dataPath + p_id + "/lum1/t6_4");
    eyePaths.push_back(dataPath + p_id + "/lum1/t7_1");
    eyePaths.push_back(dataPath + p_id + "/lum1/t7_2");
    eyePaths.push_back(dataPath + p_id + "/lum1/t7_3");
    eyePaths.push_back(dataPath + p_id + "/lum1/t7_4");
    eyePaths.push_back(dataPath + p_id + "/lum1/t8_1");
    eyePaths.push_back(dataPath + p_id + "/lum1/t8_2");
    eyePaths.push_back(dataPath + p_id + "/lum1/t8_3");
    eyePaths.push_back(dataPath + p_id + "/lum1/t8_4");
    eyePaths.push_back(dataPath + p_id + "/lum2/t6_1");
    eyePaths.push_back(dataPath + p_id + "/lum2/t6_2");
    eyePaths.push_back(dataPath + p_id + "/lum2/t6_3");
    eyePaths.push_back(dataPath + p_id + "/lum2/t6_4");
    eyePaths.push_back(dataPath + p_id + "/lum2/t7_1");
    eyePaths.push_back(dataPath + p_id + "/lum2/t7_2");
    eyePaths.push_back(dataPath + p_id + "/lum2/t7_3");
    eyePaths.push_back(dataPath + p_id + "/lum2/t7_4");
    eyePaths.push_back(dataPath + p_id + "/lum2/t8_1");
    eyePaths.push_back(dataPath + p_id + "/lum2/t8_2");
    eyePaths.push_back(dataPath + p_id + "/lum2/t8_3");
    eyePaths.push_back(dataPath + p_id + "/lum2/t8_4");
    eyePaths.push_back(dataPath + p_id + "/lum3/t6_1");
    eyePaths.push_back(dataPath + p_id + "/lum3/t6_2");
    eyePaths.push_back(dataPath + p_id + "/lum3/t6_3");
    eyePaths.push_back(dataPath + p_id + "/lum3/t6_4");
    eyePaths.push_back(dataPath + p_id + "/lum3/t7_1");
    eyePaths.push_back(dataPath + p_id + "/lum3/t7_2");
    eyePaths.push_back(dataPath + p_id + "/lum3/t7_3");
    eyePaths.push_back(dataPath + p_id + "/lum3/t7_4");
    eyePaths.push_back(dataPath + p_id + "/lum3/t8_1");
    eyePaths.push_back(dataPath + p_id + "/lum3/t8_2");
    eyePaths.push_back(dataPath + p_id + "/lum3/t8_3");
    eyePaths.push_back(dataPath + p_id + "/lum3/t8_4");





//    const string videoFilePath = "/Users/redeian/Desktop/untitled folder/mark_digit_lux1_dgt6_itr2_face.mp4";

    const string faceCascadePath = "/Users/redeian/Documents/data/haarcascade_frontalface_alt.xml";

    for( int i=0; i<videos.size(); ++i) {


        // If you want to pre cache the video put true (longer load time, but more control)
        // If not, put fault (good for a large video, and quick experiment)
        std::shared_ptr<Pupilware> pupilware = Pupilware::Create(false);

        auto algo = std::make_shared<PixelCount>("PC");
        algo->landmark.documentPath = eyePaths[i];
        pupilware->loadVideo(videos[i]);
        pupilware->addPupilSizeAlgorithm(algo);

//    pupilware->addPupilSizeAlgorithm(std::make_shared<MaximumCircleFit>("Max"));
//        pupilware->addPupilSizeAlgorithm(std::make_shared<MaximumCircleFit2>("Max1",0)); // 0 is mean
//        pupilware->addPupilSizeAlgorithm(std::make_shared<MaximumCircleFit2>("Max2",1)); // 1 is softmax
//    pupilware->addPupilSizeAlgorithm(std::make_shared<BlinkDetection>("Blink"));
//    pupilware->addPupilSizeAlgorithm(std::make_shared<MDStarbustNeo>("MDStarbustNeo"));
//    pupilware->addPupilSizeAlgorithm(std::make_shared<MDStarbustFuzzy>("Fuzzy"));

        pupilware->execute(std::make_shared<SimpleImageSegmenter>(faceCascadePath));
    }
    return 0;
}




