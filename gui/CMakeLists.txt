find_package(SDL3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(gui WIN32
    main.cpp
    asset_browser.cpp
    asset_browser.h
    asset_info.cpp
    asset_info.h
    main_window.cpp
    main_window.h
    state.cpp
    state.h
)
target_link_libraries(gui
    PRIVATE OpenGL::GL
    PRIVATE imgui
    PRIVATE core
    PRIVATE SDL3::SDL3
)
