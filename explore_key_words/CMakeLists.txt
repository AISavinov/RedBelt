cmake_minimum_required(VERSION 3.15)
project(explore_key_words)

set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CXX_STANDARD 17)

add_executable(explore_key_words main.cpp profile.h)