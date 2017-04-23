cmake_minimum_required(VERSION 3.6)
project(PromiseFuture)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(PromiseFuture ${SOURCE_FILES})
target_link_libraries (PromiseFuture -pthread)