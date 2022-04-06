cmake_minimum_required(VERSION 3.10)

# set the project name
project(FinalProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include OpenCV
include_directories(dependencies/)
set(OPENCV_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/opencv4)
set(OPENCV_LIB_DIR ${CMAKE_SOURCE_DIR}/dependencies/opencvlib)
# find_package(OpenCV 4 REQUIRED)
# include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(src/)
set(CMAKE_BUILD_TYPE Release)

aux_source_directory(src SOURCE)

# add the executable
add_executable(FinalProject src/main.cpp ${SOURCE})
target_include_directories(FinalProject PUBLIC ${OPENCV_INCLUDE_DIR})
target_link_libraries(FinalProject ${OPENCV_LIB_DIR})
target_link_libraries(FinalProject opencv_imgcodecs opencv_core opencv_imgproc)