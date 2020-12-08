cmake_minimum_required(VERSION 3.1)

set(CMAKE_CONFIGURATION_TYPES "Release;Debug")
if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()


project(
    paperlayer
    VERSION 2.0
    LANGUAGES CXX)
    
    



set(root_dir  ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(library)
add_subdirectory(generateRotations)
