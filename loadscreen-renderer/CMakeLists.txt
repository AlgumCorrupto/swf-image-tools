add_library(lrenderer STATIC
    lrenderer.cpp
    lrenderer.h
)

target_link_libraries(lrenderer
    PUBLIC OpenGL::GL
    #PRIVATE nanovg
    PUBLIC SDL3::SDL3
    PUBLIC core
)

target_include_directories(lrenderer
    ${CMAKE_CURRENT_SOURCE_DIR}
)