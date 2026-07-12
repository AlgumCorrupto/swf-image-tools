cmake_minimum_required(VERSION 4.3.4)

project(image-extract-imgui C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory(3rdparty/rswf)
add_subdirectory(3rdparty/imgui)
add_subdirectory(3rdparty/lodepng)
add_subdirectory(3rdparty/imagequant)
add_subdirectory(core)
add_subdirectory(gui)

if(TARGET quant)
    message(STATUS "quant target exists")
else()
    message(FATAL_ERROR "quant target does NOT exist")
endif()

get_target_property(loc quant IMPORTED_LOCATION)
message(STATUS "quant location = ${loc}")
