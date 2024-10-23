cmake_minimum_required(VERSION 3.10)

# set compiler
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

# set the project name
project(minifish_and_test)

file(GLOB src_files ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM src_files ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/test)