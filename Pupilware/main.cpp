#include <opencv2/objdetect/objdetect.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

//----- Include Pupilware Algorithm here -----

#include "Pupilware.hpp"
#include "Algorithm/MDStarbustNeo.hpp"
#include "Algorithm/TwoLevelSnake.hpp"

#include "ImageProcessing/SimpleImageSegmenter.hpp"

//--------------------------------------------

using namespace cv;
using namespace std;
using namespace pw;

//--------------------------------------------

/**
 * @function main
 */
int main(int argc, const char **argv) {

    std::string inputVideo = "";
    std::string outputPath = ".";

    if( argv[1] != nullptr ){
        inputVideo = argv[1];
    } else{
        std::cout << "+( +_+ )+ Please specify your video file." << std::endl;
        return 1;
    }

    if( argv[2] != nullptr ){
        outputPath = argv[2];
    }

    std::cout << "Input file is " << inputVideo << std::endl;
    std::cout << "Write files to " << outputPath << std::endl;
    std::cout << "(+_-)-> (Got it. Press 'q' to quite.) " << std::endl;

//    std::cerr << "Open CV version " << cv::getBuildInformation();


    const string faceCascadePath = "./models/haarcascade_frontalface_alt.xml";
    const string facialLandmarkPath = "./models/shape_predictor_68_face_landmarks.dat";

    // If you want to pre cache the video put true (longer load time, but more control)
    // If not, put fault (good for a large video, and quick experiment)
    std::shared_ptr<Pupilware> pupilware = Pupilware::Create(false);


    if(!pupilware->loadVideo( inputVideo )){
        std::cout << inputVideo<<" is not a video file?";
        return 0;
    }

    if(!pupilware->loadFacialLandmarkModel(facialLandmarkPath)){
        std::cout << "Error while loading land mark file";
        return 0;
    }

    pupilware->setEyeOutputPath(outputPath);

    pupilware->addPupilSizeAlgorithm(std::make_shared<TwoLevelSnake>("2LevelSnakes", 0)); // 0 is mean
    pupilware->addPupilSizeAlgorithm(std::make_shared<MDStarbustNeo>("NewStarburst"));

    pupilware->execute(std::make_shared<SimpleImageSegmenter>(faceCascadePath));

    return 0;
}




