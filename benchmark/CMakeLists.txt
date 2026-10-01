cmake_minimum_required(VERSION 3.22)
project(botsort_benchmark VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(${PROJECT_NAME}
    botsort_benchmark.cpp
    ../cpp/BoTSORT.cpp
    ../cpp/GlobalMotionCompensation.cpp
    ../cpp/KalmanFilter.cpp
    ../cpp/lapjv.cpp
    ../cpp/matching.cpp
    ../cpp/track.cpp
    ../cpp/utils.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ../cpp
    ./nitro_mirror/include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Eigen3::Eigen
    ${OpenCV_LIBS}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE PROFILE=1 O3)