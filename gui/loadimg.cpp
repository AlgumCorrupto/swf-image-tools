#include "loadimg.h"
#include <imgui.h>
#include "state.h"
#include <lrenderer.h>
#include <SDL3/SDL.h>

#include <nfd.h>

namespace loadimg {

    void replace_loadscreen() {
        nfdu8char_t* out_path = nullptr;

        nfdu8filteritem_t filters[] = {
            { "PNG image", "png" }
        };

        nfdresult_t result = NFD_OpenDialogU8(
            &out_path,
            filters,
            1,
            nullptr
        );

        if (result == NFD_OKAY) {
            try {
                apps.replace_loadscreen(out_path);
            }
            catch (const std::runtime_error& e) {
                SDL_ShowSimpleMessageBox(
                    SDL_MESSAGEBOX_ERROR,
                    "Oops",
                    e.what(),
                    sdls.w
                );
            }

            NFD_FreePathU8(out_path);
        }
        else if (result == NFD_ERROR) {
            SDL_ShowSimpleMessageBox(
                SDL_MESSAGEBOX_ERROR,
                "NFD Error",
                NFD_GetError(),
                sdls.w
            );
        }
    }

    void draw() {
        ImGui::Begin("Loading image replacer");

        const float scale = 0.5f;

        if (apps.left_loading != -1 || apps.right_loading != -1) {
            auto* dl = ImGui::GetWindowDrawList();

            ImVec2 p = ImGui::GetCursorScreenPos();

            constexpr float aspect = 16.0f / 9.0f;

            float preview_h = lrenderer::LSCR_H * scale * 2.0f;
            float preview_w = preview_h * aspect;

            float tile_h = preview_h;
            float tile_w = preview_w / 2.0f;

            if (apps.left_loading != -1) {
                dl->AddImage(
                    (ImTextureID)(intptr_t)apps.images[apps.left_loading].gl_texture,
                    p,
                    ImVec2(p.x + tile_w, p.y + tile_h)
                );
            }

            if (apps.right_loading != -1) {
                dl->AddImage(
                    (ImTextureID)(intptr_t)apps.images[apps.right_loading].gl_texture,
                    ImVec2(p.x + tile_w - LSCR_OFFS * scale, p.y),
                    ImVec2(p.x + tile_w * 2 - LSCR_OFFS * scale,
                           p.y + tile_h)
                );
            }

            ImGui::Dummy(ImVec2(preview_w, preview_h));
        }

        ImGui::BeginDisabled(apps.left_loading == -1 || apps.right_loading == -1);

        if (ImGui::Button("Replace loadscreen (image must be 16:9)"))
            replace_loadscreen();

        ImGui::EndDisabled();

        ImGui::End();
    }

    void init() {}
}