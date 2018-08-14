
cmake_minimum_required(VERSION 3.4.1)

set(PROJECT_NAME GPUImage-CP)

project(${PROJECT_NAME})

add_library(gpuimage-cp-lib SHARED

        # Provides a relative path to your source file(s).
        ../../../library/src/cpp/GPCP-Common.cpp
        ../../../library/src/cpp/GLProgram.cpp)

# android libs
find_library(log-lib log)

find_library(OpenSLES-lib OpenSLES)

find_library(GLESv2-lib GLESv2)

target_link_libraries( # Specifies the target library.
        gpuimage-cp-lib

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib} ${OpenSLES-lib} ${GLESv2-lib})