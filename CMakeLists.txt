cmake_minimum_required(VERSION 3.28)
project(advent_of_code)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexperimental-library")

add_library(trak INTERFACE)
target_sources(trak INTERFACE
        include/trak/input.h
        include/trak/graph.h)
target_include_directories(trak INTERFACE include)

add_subdirectory(src/2023)