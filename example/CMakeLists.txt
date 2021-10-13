# CMakeList.txt : CMake project for Example, include source and define
# project specific logic here.
#

cmake_minimum_required(VERSION 3.10)

# set the project name
project(example)

# add the executable
add_executable(${PROJECT_NAME} src/main.cpp)

include_directories("${PROJECT_SOURCE_DIR}/../include")

target_link_libraries(${PROJECT_NAME} kwserver)

# target_link_libraries(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/../src")

# target_include_directories(${PROJECT_NAME} PUBLIC
#                            "${PROJECT_BINARY_DIR}"
#                            "${PROJECT_SOURCE_DIR}/../include")