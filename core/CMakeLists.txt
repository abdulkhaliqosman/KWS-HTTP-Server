# CMakeList.txt : CMake project for Core, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

add_library(Core 
            "${CMAKE_CURRENT_SOURCE_DIR}/logger/logger.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/thread/threadpool.cpp"
            )

target_include_directories (Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})