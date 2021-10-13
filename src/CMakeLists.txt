# CMakeList.txt : CMake project for kwserver, include source and define
# project specific logic here.
#

cmake_minimum_required (VERSION 3.10)

# set the project name
project(kwserver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(core)

include_directories("${PROJECT_SOURCE_DIR}/../include")

add_library(kwserver STATIC
            "${CMAKE_CURRENT_SOURCE_DIR}/kwserver.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/platform/kwserver_posix.cpp"
            )


# add_subdirectory(netcore)
target_include_directories (core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} core)