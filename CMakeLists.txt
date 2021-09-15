cmake_minimum_required(VERSION 3.10)

project(kwserver)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(kwserver main.cpp)

target_link_libraries(kwserver PRIVATE Threads::Threads)
