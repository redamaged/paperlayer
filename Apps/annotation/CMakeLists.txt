cmake_minimum_required(VERSION 3.15)

project(annotations
        VERSION 1.0
        DESCRIPTION "Application to draw annotations using mouse"
        LANGUAGES CXX)

find_package( OpenCV REQUIRED )

add_executable( annotations main.cpp)
target_include_directories(annotations PUBLIC ${root_dir}/library)
target_link_libraries( annotations ${OpenCV_LIBS} imageProcessing )
