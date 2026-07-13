cmake_minimum_required(VERSION 4.3.4)

find_package(SDL3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

set(IMGUI_SAUCES
    imconfig.h
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_internal.h
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp
    imgui.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    backends/imgui_impl_sdl3.cpp
    backends/imgui_impl_sdl3.h
    backends/imgui_impl_opengl3.h
    backends/imgui_impl_opengl3.cpp
    )

add_library(imgui ${IMGUI_SAUCES})

target_include_directories(imgui
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(imgui
    PUBLIC SDL3::SDL3
    PUBLIC OpenGL::GL
)
