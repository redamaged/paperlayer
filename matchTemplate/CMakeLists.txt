cmake_minimum_required(VERSION 3.15)

project(matchTemplate
        VERSION 1.0
        DESCRIPTION "Test Match Template Algos"
        LANGUAGES CXX)

find_package( OpenCV REQUIRED )

add_executable( matchTemplate main.cpp)
target_include_directories(matchTemplate PUBLIC ${root_dir}/library)
target_link_libraries( matchTemplate ${OpenCV_LIBS} imageProcessing )
