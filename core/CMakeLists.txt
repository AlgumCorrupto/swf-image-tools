
add_library(core STATIC
    core.cpp
    core.h
)

target_include_directories(core
    PUBLIC
        ./
)

target_link_libraries(core
    PUBLIC rswf
    PRIVATE lodepng
    PRIVATE quant
)
