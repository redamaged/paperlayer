cmake_minimum_required(VERSION 3.0)
project( DisplayImage )
find_package( OpenCV REQUIRED )

add_library( imageProcessing STATIC helpers.cpp helpers.hpp )
target_include_directories(imageProcessing PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(imageProcessing ${OpenCV_LIBS} )

