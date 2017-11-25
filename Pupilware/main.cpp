#include <opencv2/opencv.hpp>

// Include Pupilware Algorithm here -----
#include "Pupilware.hpp"
#include "Algorithm/MDStarbustNeo.hpp"
#include "Algorithm/TwoLevelSnake.hpp"
#include "ImageProcessing/SimpleImageSegmenter.hpp"

//--------------------------------------

using namespace cv;
using namespace std;
using namespace pw;

//--------------------------------------

/**
 * @function main
 */
int main(int argc, const char **argv) {

    std::cout << cv::getBuildInformation() << std::endl;

    const string faceCascadePath        = "/Users/redeian/Documents/data/haarcascade_frontalface_alt.xml";
    const string video                  = "/Volumes/MarkBits/Pupilware/participantvideos/v513.mp4";
    const string outputPath             = "/Users/redeian/Documents/";


    // If you want to pre cache the video put true (longer load time, but more control)
    // If not, put fault (good for a large video, and quick experiment)
    std::shared_ptr<Pupilware> pupilware = Pupilware::Create(false);

    // Set output
    pupilware->loadVideo(video);

    pupilware->addPupilSizeAlgorithm(std::make_shared<MDStarbustNeo>("NewStarburst_P01", 0.014f, 0.1f));
    pupilware->addPupilSizeAlgorithm(std::make_shared<TwoLevelSnake>("TheSnake_P01", 0.1f, 3, 3));

    pupilware->execute(std::make_shared<SimpleImageSegmenter>(faceCascadePath));

    pupilware->saveToFiles(outputPath);

    return 0;
}




