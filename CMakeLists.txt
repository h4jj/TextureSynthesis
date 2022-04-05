cmake_minimum_required(VERSION 3.10)

# set the project name
project(FinalProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include OpenCV
find_package(OpenCV 4 REQUIRED PATHS src/)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(src/)

set(CMAKE_BUILD_TYPE Release)

aux_source_directory(src SOURCE)

# add the executable
add_executable(FinalProject src/main.cpp ${SOURCE})
target_link_libraries(FinalProject ${OpenCV_LIBS})