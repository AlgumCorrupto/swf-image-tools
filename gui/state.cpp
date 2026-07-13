#include "state.h"
#include <core.h>

#include <SDL3/SDL_opengl.h>
#include <lrenderer.h>

SdlState sdls;
AppState apps;

void Image::gen_textures() {
    if (!SDL_GL_MakeCurrent(sdls.w, sdls.gl))
    {
        printf("SDL_GL_MakeCurrent failed: %s\n", SDL_GetError());
    }
    else
    {
        printf("Current context = %p\n", SDL_GL_GetCurrentContext());
    }
    if(gl_texture != 0)
        glDeleteTextures(1, &gl_texture);

    static_assert(sizeof(RGBAColor) == 4);
    auto rgba_tex = core::RgbaImage(image.image);

    for (auto& c : rgba_tex.colors)
        if(c.a != 0)
            c.a = c.a * 2 - 1;

    glGenTextures(1, &gl_texture);
    printf("Current context = %p\n", SDL_GL_GetCurrentContext());
    printf("Expected context = %p\n", sdls.gl);
    glBindTexture(GL_TEXTURE_2D, gl_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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
    
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
        printf("OpenGL error: %x\n", err);

    printf("%u\n", gl_texture);
    printf("%d\n", glIsTexture(gl_texture));

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
    left_loading = -1;
    right_loading = -1;
}

void AppState::replace_loadscreen(std::string fname) {
    core::RgbaImage rgba(fname);

    float aspect = static_cast<float>(rgba.width) / rgba.height;

    constexpr float eps = 0.01f;

    bool valid =
        std::abs(aspect - (float)lrenderer::SCREEN_16_9_W / lrenderer::SCREEN_16_9_H) < eps;

    if (!valid)
        throw std::runtime_error("Loadscreen must be 16:9 or 4:3");

    auto result = lrenderer::render(
        rgba,
        0,
        0,
        rgba.width,
        rgba.height
    );

    images[left_loading].image.image =
        core::IndexedImage(
            std::get<0>(result),
            images[left_loading].image.image.color_table.size()
        );

    images[right_loading].image.image =
        core::IndexedImage(
            std::get<1>(result),
            images[right_loading].image.image.color_table.size()
        );

    images[left_loading].image.write();
    images[right_loading].image.write();

    images[left_loading].gen_textures();
    images[right_loading].gen_textures();
}

void AppState::close_pck() {
    images.clear();          // Destroy GL textures while the context is alive.
    core::unload_pck();
    pck_loaded = false;
    current_selected = -1;
    left_loading = -1;
    right_loading = -1;
}