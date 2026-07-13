#pragma once

#include <SDL3/SDL.h>
#include <core.h>
#include <SDL3/SDL_opengl.h>

struct SdlState {
    SDL_Window* w;
    SDL_GLContext gl;
    float main_scale;
};

struct Image {
    core::Ps2Image image;
    GLuint gl_texture = 0;

    Image(core::Ps2Image img);

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    Image(Image&& other) noexcept
        : image(std::move(other.image)),
          gl_texture(other.gl_texture)
    {
        other.gl_texture = 0;
    }

    Image& operator=(Image&& other) noexcept
    {
        if (this != &other) {
            if (gl_texture)
                glDeleteTextures(1, &gl_texture);

            image = std::move(other.image);
            gl_texture = other.gl_texture;
            other.gl_texture = 0;
        }
        return *this;
    }

    ~Image()
    {
        if (gl_texture)
            glDeleteTextures(1, &gl_texture);
    }

    void gen_textures();
};

struct AppState {
    bool pck_loaded = false;
    std::vector<Image> images;
    int current_selected = -1;
    int left_loading = -1;
    int right_loading = -1;

    void open_pck(std::string fname);
    void reload_pck();
    void close_pck();
    void save_pck(std::string fname);
    void rip_image(std::string fname);
    void replace_loadscreen(std::string fname);
};

extern AppState apps;
extern SdlState sdls;