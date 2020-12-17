cmake_minimum_required(VERSION 3.15)

project(puzzle
        VERSION 1.0
        DESCRIPTION "Image dividing app"
        LANGUAGES CXX)

find_package( OpenCV REQUIRED )

add_executable( puzzle main.cpp)
target_include_directories(puzzle PUBLIC ${root_dir}/library)
target_link_libraries( puzzle ${OpenCV_LIBS} imageProcessing )
