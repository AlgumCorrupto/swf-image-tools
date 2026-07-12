
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
    PUBLIC SDL3::SDL3
    PRIVATE lodepng
    PRIVATE quant
)

if(WIN32)
    target_link_libraries(core
        PRIVATE ws2_32
        PRIVATE ntdll
        PRIVATE userenv
    )
endif()