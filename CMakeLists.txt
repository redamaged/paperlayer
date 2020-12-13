cmake_minimum_required(VERSION 3.15)

project(
    paperlayer
    VERSION 2.0
    DESCRIPTION "Project hosting the image processing algos, utility apps, and main app for paper snippet laying"
    LANGUAGES CXX)
    
    
set(root_dir  ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(library)
add_subdirectory(generateRotations)
add_subdirectory(matchTemplate)
add_subdirectory(annotation)
