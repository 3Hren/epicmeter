CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(PROJECT ticktack)

SET(CMAKE_BUILD_TYPE "Release")

INCLUDE_DIRECTORIES(
    include
)

PROJECT(${PROJECT})

ADD_LIBRARY(ticktack SHARED
    src/benchmark
    src/printer/table
)

ADD_LIBRARY(ticktack-main
    src/main
)

ADD_DEFINITIONS(-std=c++0x)
