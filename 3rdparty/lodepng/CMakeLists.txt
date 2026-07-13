cmake_minimum_required(VERSION 3.23)

add_library(lodepng STATIC
    lodepng.cpp
    lodepng.h
)

target_include_directories(lodepng
    PUBLIC
        ./
)