#include "asset_info.h"
#include <imgui.h>
#include "state.h"

#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_dialog.h>

void replace_selected_cb(void* userdata, const char * const * filelist, int filter) {
    if(filelist == NULL)
        return;
    if(*filelist == NULL)
        return;
    try {
        apps.images[apps.current_selected].image.replace(std::string(filelist[0]));
        apps.images[apps.current_selected].gen_textures();
    } catch(std::runtime_error(e)) {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Image does not match",
            "Input image needs to be the same resolution as output",
            sdls.w
        );
    }
    return;
}

void replace_image() {
    SDL_DialogFileFilter filters  = {"Png images (*.png)", "png"};
    SDL_ShowOpenFileDialog(
        replace_selected_cb,
        NULL,
        sdls.w,
        &filters,
        1,
        "./",
        false
    );
}

void rip_image_cb(void* userdata, const char * const * filelist, int filter) {
    if(filelist == NULL)
        return;
    if(*filelist == NULL)
        return;
    try {
        apps.rip_image(std::string(filelist[0]));
    } catch(std::runtime_error(e)) {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Image does not match",
            "Input image needs to be the same aspect ratio as output",
            sdls.w
        );
    }
    return;
}

void rip_image() {
    SDL_DialogFileFilter filters  = {"Png images (*.png)", "png"};
    SDL_ShowSaveFileDialog(
        rip_image_cb,
        NULL,
        sdls.w,
        &filters,
        1,
        "./"
    );
    }

void assetinfo::draw() {

    ImGui::Begin("Asset Info");
    if(!(apps.pck_loaded) || apps.current_selected == -1) {
        ImGui::End();
        return;
    }

    constexpr float thumb = 128.f;

    const auto& img = apps.images[apps.current_selected].image.image;
    float scale = std::min(
        thumb / img.width,
        thumb /  img.height
    );
    ImVec2 size(
        img.width * scale,
        img.height * scale
    );

    ImGui::Image((ImTextureID)(intptr_t)apps.images[apps.current_selected].gl_texture, size);

    char res_fmt[64];
    snprintf(res_fmt, sizeof(res_fmt), "resolution: %dx%d", img.width, img.height);

    ImGui::Text(res_fmt);

    std::string type;
    switch(apps.images[apps.current_selected].image.type) {
        case BPP4_UNSWIZZLED: type = "4 BPP unswizzled"; break;
        case BPP8_BOTH_SWIZZLED: type = "8 BPP palette and index swizzled"; break;
        case BPP8_INDEX_SWIZZLED_BUT_TEXTURE_NOT: type = "8 BPP palette and index unswizzled"; break;
    }
    ImGui::Text(type.c_str());

    char addr_fmt[64];
    snprintf(addr_fmt, sizeof(addr_fmt), "0x%.8x (bmpInfo1)", apps.images[apps.current_selected].image.address);
    ImGui::InputText("##base_addr", addr_fmt, IM_ARRAYSIZE(addr_fmt), ImGuiInputTextFlags_ReadOnly);

    if(ImGui::Button("Replace (must be exact same aspect ratio)")) {
        replace_image();
    }
    if(ImGui::Button("Rip image")) {
        rip_image();
    }

    ImGui::End();
}

void assetinfo::init() {

}