cmake_minimum_required(VERSION 4.3.4)


project(image-extract-imgui C CXX)

set(SDL_SHARED OFF)
set(SDL_STATIC ON)    

# it aint working dawggy
# add_compile_options(-Oz -ffunction-sections -Wl,--gc-sections)

include(FetchContent)

FetchContent_Declare(
    sdl
    GIT_REPOSITORY https://github.com/libsdl-org/SDL 
    GIT_TAG release-3.4.12 
)
FetchContent_MakeAvailable(sdl)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(3rdparty/rswf)
add_subdirectory(3rdparty/imgui)
add_subdirectory(3rdparty/lodepng)
add_subdirectory(3rdparty/imagequant)
add_subdirectory(3rdparty/nanovg)
add_subdirectory(core)
add_subdirectory(gui)
