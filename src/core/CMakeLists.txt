# CMakeList.txt : CMake project for Core, include source and define
# project specific logic here.
#

cmake_minimum_required (VERSION 3.10)

# set the project name
project(core)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(core 
            "${CMAKE_CURRENT_SOURCE_DIR}/logger/logger.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/thread/threadpool.cpp"
            )

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

target_include_directories (core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})