file(GLOB VG_H src/*.h)

file(GLOB VG_SRC src/*.c)

add_library(nanovg STATIC
    ${VG_H}
    ${VG_SRC}
)

target_include_directories(nanovg
    PUBLIC src/
)
