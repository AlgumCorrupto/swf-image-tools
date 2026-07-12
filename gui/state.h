#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <core.h>
#include <SDL3/SDL_opengl.h>

struct SdlState {
    SDL_Window* w;
    SDL_GLContext gl;
    float main_scale;
};

struct Image {
    core::Ps2Image image;
    uint32_t gl_texture;
    Image(core::Ps2Image img);
    void gen_textures();
    ~Image() {
        glDeleteTextures(1, &gl_texture);
    }
};

struct AppState {
    bool pck_loaded = false;
    std::vector<Image> images;
    int current_selected = -1;

    void open_pck(std::string fname);
    void reload_pck();
    void close_pck();
    void save_pck(std::string fname);
    void rip_image(std::string fname);
};

extern AppState apps;
extern SdlState sdls;