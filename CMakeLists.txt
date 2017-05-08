cmake_minimum_required(VERSION 3.5)
project(Pupilware)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        Pupilware/Algorithm/IPupilAlgorithm.cpp
        Pupilware/Algorithm/IPupilAlgorithm.hpp
        Pupilware/Algorithm/DummyAlgo.cpp
        Pupilware/Algorithm/DummyAlgo.hpp
        Pupilware/Algorithm/MDStarbust.cpp
        Pupilware/Algorithm/MDStarbust.hpp
        Pupilware/Helpers/math/Circle.cpp
        Pupilware/Helpers/math/Circle.h
        Pupilware/Helpers/CWCVHelper.cpp
        Pupilware/Helpers/CWCVHelper.hpp
        Pupilware/Helpers/simpleGraph/GraphUtils.cpp
        Pupilware/Helpers/simpleGraph/GraphUtils.h
        Pupilware/Helpers/math/Ransac.cpp
        Pupilware/Helpers/math/Ransac.h
        Pupilware/constants.h
        Pupilware/main.cpp
        Pupilware/PWCSVExporter.hpp
        Pupilware/PWCSVExporter.cpp
        Pupilware/PWFaceLandmarkDetector.hpp
        Pupilware/PWFaceLandmarkDetector.cpp
        Pupilware/PWParameter.hpp
        Pupilware/PWParameter.cpp
        Pupilware/Algorithm/PWFaceMeta.cpp
        Pupilware/Algorithm/PWFaceMeta.hpp
        Pupilware/Pupilware.cpp
        Pupilware/Pupilware.hpp
        Pupilware/Helpers/CWUIHelper.cpp
        Pupilware/Helpers/CWUIHelper.hpp
        Pupilware/ImageProcessing/IImageSegmenter.cpp
        Pupilware/ImageProcessing/IImageSegmenter.hpp
        Pupilware/ImageProcessing/SimpleImageSegmenter.cpp
        Pupilware/ImageProcessing/SimpleImageSegmenter.hpp
        Pupilware/SignalProcessing/ISignalProcessor.cpp
        Pupilware/SignalProcessing/ISignalProcessor.hpp
        Pupilware/SignalProcessing/BasicSignalProcessor.cpp
        Pupilware/SignalProcessing/BasicSignalProcessor.hpp
        Pupilware/SignalProcessing/SignalProcessingHelper.hpp
        Pupilware/SignalProcessing/SignalProcessingHelper.cpp
        Pupilware/SignalProcessing/sgsmooth.h
        Pupilware/SignalProcessing/sgsmooth.C
        Pupilware/Helpers/CVWindow.cpp
        Pupilware/Helpers/CVWindow.hpp
        Pupilware/Helpers/PWGraph.cpp
        Pupilware/Helpers/PWGraph.hpp
        Pupilware/Algorithm/MDStarbustG.cpp
        Pupilware/Algorithm/MDStarbustG.hpp
        Pupilware/Algorithm/PWDataModel.cpp
        Pupilware/Algorithm/PWDataModel.hpp
        Pupilware/Algorithm/PWTypes.hpp
        Pupilware/Algorithm/ALOfflineFile.cpp
        Pupilware/Algorithm/ALOfflineFile.hpp
        Pupilware/Algorithm/MDStarbustNeo.cpp
        Pupilware/Algorithm/MDStarbustNeo.hpp
        Pupilware/Algorithm/MaximumCircleFit.cpp
        Pupilware/Algorithm/MaximumCircleFit.hpp
        Pupilware/Algorithm/MaximumCircleFit2.cpp
        Pupilware/Algorithm/MaximumCircleFit2.hpp
        Pupilware/Algorithm/BlinkDetection.cpp
        Pupilware/Algorithm/BlinkDetection.hpp
        Pupilware/Algorithm/PixelCount.cpp
        Pupilware/Algorithm/PixelCount.hpp
        Pupilware/Algorithm/MDStarbustFuzzy.cpp
        Pupilware/Algorithm/MDStarbustFuzzy.hpp
        Pupilware/preHeader.hpp
        Pupilware/Helpers/math/Snakuscules.cpp
        Pupilware/Helpers/math/Snakuscules.hpp)


set(OpenCV_DIR "/usr/local/Cellar/opencv3/3.1.0_3/")
set(OpenCV_INCLUDE_DIRS "${OpenCV_DIR}include")
set(OpenCV_LIBARY_DIR "${OpenCV_DIR}lib")
set(OpenCV_LIBS
        -L${OpenCV_LIBARY_DIR}
        opencv_core
        opencv_highgui
        opencv_objdetect
        opencv_imgproc
        opencv_video
        opencv_videoio
        opencv_photo
        opencv_imgcodecs)

set(DLIB_DIR "/usr/local/Cellar/dlib/19.2/")
set(DLIB_INCLUDE_DIRS "${DLIB_DIR}include")
set(DLIB_LIBARY_DIR "/Users/redeian/Documents/projects/dlib19/build")
set(DLIB_LIBS
        -L${DLIB_LIBARY_DIR}
        libdlib.a)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${DLIB_INCLUDE_DIRS})
include_directories(/Users/redeian/Documents/projects/)


#For the static library:
#set ( DLIB_LIBS libdlib.a )
link_directories( ${DLIB_INCLUDE_DIRS} )

#find_package( OpenCV REQUIRED )
add_executable( Pupilware ${SOURCE_FILES})

add_subdirectory(TestPupilwareCore)

target_link_libraries( Pupilware ${OpenCV_LIBS} ${DLIB_LIBS})

