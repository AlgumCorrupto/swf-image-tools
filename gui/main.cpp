#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl2.h>
#include <SDL3/SDL_opengl.h>
#include <SDL2/SDL_video.h>

#include "state.h"
#include "main_window.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO);
    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    sdls.main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    sdls.w = SDL_CreateWindow("Who read this is gay", (int)(1280 * sdls.main_scale), (int)(800 * sdls.main_scale), window_flags);
    sdls.gl = SDL_GL_CreateContext(sdls.w);
    SDL_GL_MakeCurrent(sdls.w, sdls.gl);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(sdls.main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = sdls.main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
    io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(sdls.w, sdls.gl);
    ImGui_ImplOpenGL2_Init();

    mainwindow::init();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    ImGui_ImplSDL3_ProcessEvent(event);

    switch (event->type)
    {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;

        // Handle your own events here if needed.
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

// Update and Render every frame
SDL_AppResult SDL_AppIterate(void *appstate) {
    if (SDL_GetWindowFlags(sdls.w) & SDL_WINDOW_MINIMIZED)
        SDL_Delay(10);
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    mainwindow::draw();
    ImGui::Render();

    ImGuiIO& io = ImGui::GetIO();
    glViewport(0,0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0,0,0,255);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(sdls.w);
    return SDL_APP_CONTINUE;
}

// Clean up before exiting
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    apps.close_pck();
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DestroyContext(sdls.gl);
    SDL_DestroyWindow(sdls.w);
    SDL_Quit();
}
