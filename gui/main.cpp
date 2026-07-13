#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengl.h>

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>

#include "state.h"
#include "main_window.h"


SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        return SDL_APP_FAILURE;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    sdls.main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

    SDL_WindowFlags window_flags =
        SDL_WINDOW_OPENGL |
        SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_HIGH_PIXEL_DENSITY;

    sdls.w = SDL_CreateWindow(
        "Who read this is gay",
        (int)(1280 * sdls.main_scale),
        (int)(800 * sdls.main_scale),
        window_flags
    );

    if (!sdls.w)
        return SDL_APP_FAILURE;


    sdls.gl = SDL_GL_CreateContext(sdls.w);

    if (!sdls.gl)
        return SDL_APP_FAILURE;

    SDL_GL_MakeCurrent(sdls.w, sdls.gl);
    SDL_GL_SetSwapInterval(1);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


    ImGui::StyleColorsDark();


    ImGuiStyle& style = ImGui::GetStyle();

    style.ScaleAllSizes(sdls.main_scale);
    style.FontScaleDpi = sdls.main_scale;

    io.ConfigDpiScaleFonts = true;
    io.ConfigDpiScaleViewports = true;


    ImGui_ImplSDL3_InitForOpenGL(sdls.w, sdls.gl);

    // OpenGL 3 backend
    ImGui_ImplOpenGL3_Init(nullptr);


    mainwindow::init();

    return SDL_APP_CONTINUE;
}



SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    ImGui_ImplSDL3_ProcessEvent(event);

    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    return SDL_APP_CONTINUE;
}



SDL_AppResult SDL_AppIterate(void *appstate)
{
    if (SDL_GetWindowFlags(sdls.w) & SDL_WINDOW_MINIMIZED)
        SDL_Delay(10);


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();

    mainwindow::draw();

    ImGui::Render();


    ImGuiIO& io = ImGui::GetIO();

    glViewport(
        0,
        0,
        (int)io.DisplaySize.x,
        (int)io.DisplaySize.y
    );

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);


    ImGui_ImplOpenGL3_RenderDrawData(
        ImGui::GetDrawData()
    );


    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }


    SDL_GL_SwapWindow(sdls.w);

    return SDL_APP_CONTINUE;
}



void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    apps.close_pck();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();

    ImGui::DestroyContext();

    SDL_GL_DestroyContext(sdls.gl);
    SDL_DestroyWindow(sdls.w);

    SDL_Quit();
}