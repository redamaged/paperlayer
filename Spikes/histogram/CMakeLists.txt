cmake_minimum_required(VERSION 3.15)

project(histogram
        VERSION 1.0
        DESCRIPTION "Application to test OpenCV histograms"
        LANGUAGES CXX)

find_package( OpenCV REQUIRED )

add_executable( histogram main.cpp)
target_include_directories(histogram PUBLIC ${root_dir}/library)
target_link_libraries( histogram ${OpenCV_LIBS} imageProcessing )
