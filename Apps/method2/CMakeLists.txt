cmake_minimum_required(VERSION 3.15)

project(method2
        VERSION 1.0
        DESCRIPTION "Application to to divide a ref image, template match its parts with list of images, and collate the matches in a collage image"
        LANGUAGES CXX)

find_package( OpenCV REQUIRED )

add_executable( method2 main.cpp)
target_include_directories(method2 PUBLIC ${root_dir}/library)
target_link_libraries( method2 ${OpenCV_LIBS} imageProcessing )
