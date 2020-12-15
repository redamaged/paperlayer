cmake_minimum_required(VERSION 3.15)

project(processPatches
        VERSION 1.0
        DESCRIPTION "Application to paper lay all saved patches in a given dir"
        LANGUAGES CXX)

find_package( OpenCV REQUIRED )

add_executable( processPatches main.cpp)
target_include_directories(processPatches PUBLIC ${root_dir}/library)
target_link_libraries( processPatches ${OpenCV_LIBS} imageProcessing )
