cmake_minimum_required(VERSION 3.15)

project(parallelFor
        VERSION 1.0
        DESCRIPTION "Test OpenCV parallel framework"
        LANGUAGES CXX)

find_package( OpenCV REQUIRED )

add_executable( parallelFor main.cpp)
target_include_directories(parallelFor PUBLIC ${root_dir}/library)
target_link_libraries( parallelFor ${OpenCV_LIBS} imageProcessing )
