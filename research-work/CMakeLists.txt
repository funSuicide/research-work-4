cmake_minimum_required(VERSION 2.8)
project(mainProject)

SET(MYCOMPILE_FLAGS "-O3 -march=native -std=c++20 -mavx2 -mavx512dq")

add_definitions(${MYCOMPILE_FLAGS})

add_executable(${PROJECT_NAME} research-work.cpp MagmaAVX2.cpp Kuznechik.cpp Structures.cpp MagmaAVX512.cpp)	




