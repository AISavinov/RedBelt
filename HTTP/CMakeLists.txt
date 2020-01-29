cmake_minimum_required(VERSION 3.15)
project(HTTP)

set(CMAKE_CXX_STANDARD 17)

add_executable(HTTP server_stats.cpp test_runner.h http_request.h stats.h stats.cpp)