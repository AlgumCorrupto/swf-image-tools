#include "state.h"
#include <core.h>

#include <SDL3/SDL_opengl.h>

SdlState sdls;
AppState apps;

void Image::gen_textures() {
    if(gl_texture != 0)
        glDeleteTextures(1, &gl_texture);

    auto rgba_tex = core::RgbaImage(image.image);
    for (auto& c : rgba_tex.colors)
        c.a = (c.a > 1) ? 255 : 0;

    glGenTextures(1, &gl_texture);
    glBindTexture(GL_TEXTURE_2D, gl_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        rgba_tex.width,
        rgba_tex.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        rgba_tex.colors.data()
    );

    glBindTexture(GL_TEXTURE_2D, 0);
}

Image::Image(core::Ps2Image img) : image(std::move(img)) {
    gen_textures();
}

void AppState::save_pck(std::string fname) {
    core::save_pck(fname);
}

void AppState::rip_image(std::string fname) {
    images[current_selected].image.image.save(fname);
}

void AppState::open_pck(std::string fname) {
    if (pck_loaded)
        close_pck();

    try {
        core::load_pck(fname);
        reload_pck();
    } catch (const std::runtime_error& e) {
        printf("%s\n", e.what());
    }
}

void AppState::reload_pck() {
    auto ps2_images = core::get_images();

    std::vector<Image> new_images;
    new_images.reserve(ps2_images.size());

    for (auto& ps2_img : ps2_images)
        new_images.emplace_back(std::move(ps2_img));

    images = std::move(new_images);
    pck_loaded = true;
    current_selected = -1;
}

void AppState::close_pck() {
    images.clear();          // Destroy GL textures while the context is alive.
    core::unload_pck();
    pck_loaded = false;
    current_selected = -1;
}