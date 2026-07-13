#include "loadimg.h"
#include <imgui.h>
#include "state.h"
#include <lrenderer.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_dialog.h>

namespace loadimg {

    void replace_loadscreen_cb(void *userdata, const char *const *filelist, int filter) {

        if(filelist == NULL)
            return;
        if(*filelist == NULL)
            return;
        try{apps.replace_loadscreen(filelist[0]);}
        catch(std::runtime_error e) {
            SDL_ShowSimpleMessageBox(
                SDL_MESSAGEBOX_ERROR,
                "Oops",
                e.what(),
                sdls.w
            );
        }
        return;
    }



    void replace_loadscreen() {
        SDL_DialogFileFilter filters = {
            "2:1 PNG image (*.png)", "png"
        };
        SDL_ShowOpenFileDialog(
            replace_loadscreen_cb,
            NULL,
            sdls.w,
            &filters,
            1,
            "./",
            false
        );

    }

    void draw() {
        ImGui::Begin("Loading image replacer");

        const float scale = 0.5f;

        if(apps.left_loading != -1 || apps.right_loading != -1) {
            auto dl = ImGui::GetWindowDrawList();

            ImVec2 p = ImGui::GetCursorScreenPos();

            // Render preview as 16:9
            constexpr float aspect = 16.0f / 9.0f;

            float preview_h = lrenderer::LSCR_H * scale * 2.0f;
            float preview_w = preview_h * aspect;

            float tile_h = preview_h;
            float tile_w = preview_w / 2.0f;

            if(apps.left_loading != -1) {
                ImVec2 pl_min = p;
                ImVec2 pl_max(
                    p.x + tile_w,
                    p.y + tile_h
                );

                dl->AddImage(
                    (ImTextureRef)(uintptr_t)apps.images[apps.left_loading].gl_texture,
                    pl_min,
                    pl_max
                );
            }

            if(apps.right_loading != -1) {
                ImVec2 pr_min(
                    p.x + tile_w - LSCR_OFFS * scale,
                    p.y
                );

                ImVec2 pr_max(
                    p.x + tile_w * 2 - LSCR_OFFS * scale,
                    p.y + tile_h
                );

                dl->AddImage(
                    (ImTextureRef)(uintptr_t)apps.images[apps.right_loading].gl_texture,
                    pr_min,
                    pr_max
                );
            }

            ImGui::Dummy(ImVec2(preview_w, preview_h));
        }

        ImGui::BeginDisabled(apps.left_loading != 1 && apps.right_loading != 1);

        if(ImGui::Button("Replace loadscreen (image must be 16:9)")) {
            replace_loadscreen();
        }

        ImGui::EndDisabled();

        ImGui::End();
    }
    void init();
}