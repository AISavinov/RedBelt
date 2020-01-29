cmake_minimum_required(VERSION 3.15)
project(concurrent_map)

set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CXX_STANDARD 17)

add_executable(concurrent_map main.cpp)