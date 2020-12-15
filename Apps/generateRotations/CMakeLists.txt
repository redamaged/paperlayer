cmake_minimum_required(VERSION 3.15)

project(generateRotations
        VERSION 1.0
        DESCRIPTION "Application to rotate an input image and save the transparent rotations into files"
        LANGUAGES CXX)

find_package( OpenCV REQUIRED )

add_executable( generateRotations main.cpp)
target_include_directories(generateRotations PUBLIC ${root_dir}/library)
target_link_libraries( generateRotations ${OpenCV_LIBS} imageProcessing )
