cmake_minimum_required(VERSION 3.15)

project(imageProcessing
        VERSION 1.0
        DESCRIPTION "Library for Image Processing Functionalities"
        LANGUAGES CXX)


find_package( OpenCV REQUIRED )

add_library( imageProcessing STATIC helpers.cpp helpers.hpp algos.cpp algos.hpp)
target_include_directories(imageProcessing PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(imageProcessing ${OpenCV_LIBS} )

