cmake_minimum_required(VERSION 3.14)
project(TextEditor)

set(CMAKE_CXX_STANDARD 17)

add_executable(TextEditor main.cpp test_runner.h test.h)