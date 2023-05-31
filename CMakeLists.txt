cmake_minimum_required(VERSION 3.9.0)
project(vtk)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} 
            main.cpp
            helpers.cpp 
            tlv.cpp 
            vtk.cpp)