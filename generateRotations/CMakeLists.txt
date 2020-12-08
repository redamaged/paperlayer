cmake_minimum_required(VERSION 3.0)


project(generateRotations)

find_package( OpenCV REQUIRED )

include_directories(${root_dir}/library)

add_executable( generateRotations main.cpp)
target_link_libraries( generateRotations ${OpenCV_LIBS} imageProcessing )
