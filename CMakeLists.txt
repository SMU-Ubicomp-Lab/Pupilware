cmake_minimum_required(VERSION 3.5)
project(Pupilware)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        Pupilware/Algorithm/PWAlgorithm.cpp
        Pupilware/Algorithm/PWAlgorithm.hpp
        Pupilware/Algorithm/DummyAlgo.cpp
        Pupilware/Algorithm/DummyAlgo.hpp
        Pupilware/Algorithm/MDStarbust.cpp
        Pupilware/Algorithm/MDStarbust.hpp
        Pupilware/etc/Circle.cpp
        Pupilware/etc/Circle.h
        Pupilware/etc/CWCVUtility.cpp
        Pupilware/etc/CWCVUtility.hpp
        Pupilware/etc/GraphUtils.cpp
        Pupilware/etc/GraphUtils.h
        Pupilware/etc/helpers.cpp
        Pupilware/etc/helpers.h
        Pupilware/etc/Ransac.cpp
        Pupilware/etc/Ransac.h
        Pupilware/constants.h
        Pupilware/main.cpp
        Pupilware/PupilMeta.cpp
        Pupilware/PupilMeta.hpp
        Pupilware/Pupilware.cpp
        Pupilware/Pupilware.hpp Pupilware/etc/CWUIHelper.cpp Pupilware/etc/CWUIHelper.hpp Pupilware/ImageProcessing/IImageSegmenter.cpp Pupilware/ImageProcessing/IImageSegmenter.hpp Pupilware/ImageProcessing/SimpleImageSegmenter.cpp Pupilware/ImageProcessing/SimpleImageSegmenter.hpp Pupilware/SignalProcessing/ISignalProcessor.cpp Pupilware/SignalProcessing/ISignalProcessor.hpp Pupilware/SignalProcessing/SimpleSignalProcessor.cpp Pupilware/SignalProcessing/SimpleSignalProcessor.hpp Pupilware/SignalProcessing/SignalProcessingHelper.hpp Pupilware/SignalProcessing/SignalProcessingHelper.cpp)

find_package( OpenCV REQUIRED )
add_executable( Pupilware ${SOURCE_FILES})

target_link_libraries( Pupilware ${OpenCV_LIBS} )