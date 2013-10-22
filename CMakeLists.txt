cmake_minimum_required(VERSION 2.6)

project(muduosamples CXX)

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(ch01)
