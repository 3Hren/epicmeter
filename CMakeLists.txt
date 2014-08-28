CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(PROJECT ticktack)

# Set a consistent MACOSX_RPATH default across all CMake versions.
# When CMake 2.8.12 is required, change this default to 1.
# When CMake 3.0.0 is required, remove this block (see CMP0042).
IF(NOT DEFINED CMAKE_MACOSX_RPATH)
    SET(CMAKE_MACOSX_RPATH 0)
ENDIF()

SET(CMAKE_BUILD_TYPE "Release")

INCLUDE_DIRECTORIES(
    include
)

PROJECT(${PROJECT})

ADD_LIBRARY(ticktack SHARED
    src/benchmark
    src/printer/table
    src/printer/json
)

ADD_LIBRARY(ticktack-main
    src/main
)

ADD_DEFINITIONS(-std=c++0x)
