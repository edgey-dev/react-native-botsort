cmake_minimum_required(VERSION 3.22)
project(NitroBotsort)
set(PACKAGE_NAME NitroBotsort)

# Nitro 0.37 requires C++20 (remove_cvref_t, map::contains, and abbreviated templates).
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  Eigen3
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG        3.4.0
)
set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_BTL OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_PKGCONFIG OFF CACHE BOOL "" FORCE)
FetchContent_Populate(Eigen3)

# Eigen 3.4 always declares BLAS/LAPACK and spbench targets. Android's
# externalNativeBuild tries to compile every declared target, so remove those
# optional subdirectories before configuring Eigen itself.
file(READ "${eigen3_SOURCE_DIR}/CMakeLists.txt" EIGEN_CMAKE)
string(REPLACE
  "add_subdirectory(blas EXCLUDE_FROM_ALL)"
  "# add_subdirectory(blas EXCLUDE_FROM_ALL)"
  EIGEN_CMAKE "${EIGEN_CMAKE}")
string(REPLACE
  "add_subdirectory(lapack EXCLUDE_FROM_ALL)"
  "# add_subdirectory(lapack EXCLUDE_FROM_ALL)"
  EIGEN_CMAKE "${EIGEN_CMAKE}")
string(REPLACE
  "add_subdirectory(bench/spbench EXCLUDE_FROM_ALL)"
  "# add_subdirectory(bench/spbench EXCLUDE_FROM_ALL)"
  EIGEN_CMAKE "${EIGEN_CMAKE}")
file(WRITE "${eigen3_SOURCE_DIR}/CMakeLists.txt" "${EIGEN_CMAKE}")
add_subdirectory("${eigen3_SOURCE_DIR}" "${eigen3_BINARY_DIR}")

# motcpp does a `find_package(Eigen3 CONFIG REQUIRED)` during its own configure step.
# FetchContent gives us the Eigen build tree, but not an installed package prefix, so
# we must expose the generated config location to CMake before motcpp is configured.
if(DEFINED eigen3_BINARY_DIR)
    set(Eigen3_DIR "${eigen3_BINARY_DIR}")
    set(eigen3_DIR "${eigen3_BINARY_DIR}")
endif()
if(DEFINED Eigen3_BINARY_DIR)
    set(Eigen3_DIR "${Eigen3_BINARY_DIR}")
    set(eigen3_DIR "${Eigen3_BINARY_DIR}")
endif()
list(PREPEND CMAKE_PREFIX_PATH "${Eigen3_DIR}" "${eigen3_DIR}")
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

set(YAML_CPP_INSTALL ON CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG        0.8.0
)
FetchContent_MakeAvailable(yaml-cpp)

# motcpp calls find_package(yaml-cpp) during the same configure pass. Its
# FetchContent target already exists, so provide an immediate build-tree config
# instead of relying on yaml-cpp's deferred install/export generation.
file(WRITE "${yaml-cpp_BINARY_DIR}/yaml-cpp-config.cmake" "
if(NOT TARGET yaml-cpp::yaml-cpp)
  message(FATAL_ERROR \"yaml-cpp::yaml-cpp target not found in build tree\")
endif()
set(yaml-cpp_FOUND TRUE)
")

set(yaml-cpp_DIR "${yaml-cpp_BINARY_DIR}" CACHE PATH "" FORCE)
list(PREPEND CMAKE_PREFIX_PATH "${yaml-cpp_DIR}")
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

FetchContent_Declare(
  opencv_mobile_android
  URL https://github.com/nihui/opencv-mobile/releases/download/v35/opencv-mobile-4.13.0-android.zip
)
FetchContent_MakeAvailable(opencv_mobile_android)

set(OpenCV_DIR ${opencv_mobile_android_SOURCE_DIR}/sdk/native/jni)
find_package(OpenCV REQUIRED core imgproc calib3d)


# 1. Fetch motcpp — https://github.com/Geekgineer/motcpp
FetchContent_Declare(
  motcpp
  GIT_REPOSITORY https://github.com/Geekgineer/motcpp.git
  GIT_TAG        main
)

# ONNX Runtime Android setup for later testing:
# set(ONNXRUNTIME_VERSION "1.20.1")
# set(ONNXRUNTIME_AAR "${CMAKE_BINARY_DIR}/third_party/onnxruntime-android-${ONNXRUNTIME_VERSION}.aar")
# set(ONNXRUNTIME_ANDROID_DIR "${CMAKE_BINARY_DIR}/third_party/onnxruntime-android-${ONNXRUNTIME_VERSION}")
# set(ONNXRUNTIME_ANDROID_LIB "${ONNXRUNTIME_ANDROID_DIR}/jni/${CMAKE_ANDROID_ARCH_ABI}/libonnxruntime.so")
# file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/third_party")
# file(DOWNLOAD
#   "https://repo1.maven.org/maven2/com/microsoft/onnxruntime/onnxruntime-android/${ONNXRUNTIME_VERSION}/onnxruntime-android-${ONNXRUNTIME_VERSION}.aar"
#   "${ONNXRUNTIME_AAR}"
#   SHOW_PROGRESS
#   TLS_VERIFY ON
# )
# file(MAKE_DIRECTORY "${ONNXRUNTIME_ANDROID_DIR}")
# execute_process(
#   COMMAND "${CMAKE_COMMAND}" -E tar xf "${ONNXRUNTIME_AAR}"
#   WORKING_DIRECTORY "${ONNXRUNTIME_ANDROID_DIR}"
# )
# add_library(onnxruntime_android SHARED IMPORTED GLOBAL)
# set_target_properties(onnxruntime_android PROPERTIES
#   IMPORTED_LOCATION "${ONNXRUNTIME_ANDROID_LIB}"
#   INTERFACE_INCLUDE_DIRECTORIES "${ONNXRUNTIME_ANDROID_DIR}/headers"
# )
# target_compile_definitions(motcpp PUBLIC MOTCPP_HAS_ONNX=1)
# target_link_libraries(motcpp PRIVATE onnxruntime_android)

# Keep ONNX Runtime disabled until the Android package is tested.
set(MOTCPP_ENABLE_ONNX OFF CACHE BOOL "Disable ONNX Runtime" FORCE)
set(MOTCPP_BUILD_TESTS OFF CACHE BOOL "Disable motcpp tests" FORCE)
set(MOTCPP_BUILD_BENCHMARKS OFF CACHE BOOL "Disable motcpp benchmarks" FORCE)
set(MOTCPP_BUILD_EXAMPLES OFF CACHE BOOL "Disable motcpp examples" FORCE)
set(MOTCPP_BUILD_TOOLS OFF CACHE BOOL "Disable motcpp tools" FORCE)
set(MOTCPP_INSTALL OFF CACHE BOOL "Disable motcpp install targets" FORCE)
set(ONNXRUNTIME_FOUND FALSE CACHE BOOL "Disable ONNX Runtime discovery" FORCE)
unset(ONNXRUNTIME_INCLUDE_DIR CACHE)
unset(ONNXRUNTIME_LIBRARY CACHE)
FetchContent_MakeAvailable(motcpp)

# Define the single shared library target for this native module.
# cpp-adapter.cpp is the Nitro-generated JNI entry point; the autolinking
# include below adds your tracker source + generated OnLoad.cpp to THIS target.
add_library(${PACKAGE_NAME} SHARED
        src/main/cpp/cpp-adapter.cpp
        ../cpp/HybridBoTSortTracker.cpp
)

# Add Nitrogen specs :)
# This filename must match the generated file exactly, including case.
include(${CMAKE_SOURCE_DIR}/../nitrogen/generated/android/NitroBotsort+autolinking.cmake)

# Set up local includes
include_directories(
        "src/main/cpp"
        "../cpp"
)

find_library(LOG_LIB log)
find_package(react-native-vision-camera REQUIRED)  #<-- for the HybridFrameSpec

# Link all libraries together — single consolidated call
target_link_libraries(
        ${PACKAGE_NAME}
        ${LOG_LIB}
        android                                   # <-- Android core
        motcpp::motcpp
        react-native-vision-camera::VisionCamera
        ${OpenCV_LIBS}
)

# Export only the explicit initialization hook required by Nitro's JSI runtime loader
set_target_properties(${PACKAGE_NAME} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
)

# --- ADVANCED MODULE COMPILER SIZE STRIPPING ---
if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Keep debug symbols intact during desk testing phases
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto -fvisibility=hidden -fvisibility-inlines-hidden")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdata-sections -ffunction-sections")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl,--exclude-libs,ALL")
endif()