find_package(OpenGL REQUIRED)

add_executable(gui WIN32
    main.cpp
    asset_browser.cpp
    asset_browser.h
    asset_info.cpp
    asset_info.h
    main_window.cpp
    main_window.h
    loadimg.cpp
    loadimg.h
    state.cpp
    state.h
)

target_link_libraries(gui
    PRIVATE OpenGL::GL
    PRIVATE imgui
    PRIVATE core
    PRIVATE lrenderer
    PRIVATE SDL3::SDL3
    PRIVATE -static-libgcc
    PRIVATE -static-libstdc++
    PRIVATE nfd
)

if(WIN32)
    # Statically links GCC runtime, standard library, and winpthread
    target_link_libraries(gui PRIVATE -static winpthread -dynamic)
endif()