cmake_minimum_required(VERSION 3.10)
project(AIS2203_JetRacer)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_subdirectory(src)
add_subdirectory(executables)
