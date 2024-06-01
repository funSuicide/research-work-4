cmake_minimum_required(VERSION 3.16)
project(mainProject CXX)
if(MSVC)
	add_compile_options(/std:c++latest /arch:AVX512)
else()
	add_compile_options(-march=native -std=c++2b)
endif()


add_executable(${PROJECT_NAME} research-work.cpp MagmaAVX2.cpp Kuznechik.cpp Structures.cpp MagmaAVX512.cpp)	




