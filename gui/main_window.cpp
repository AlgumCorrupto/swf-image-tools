#include "main_window.h"

#include <SDL3/SDL.h>
#include <imgui.h>
#include <nfd.h>

#include "state.h"
#include "asset_browser.h"
#include "asset_info.h"
#include "loadimg.h"

namespace mainwindow {

    void load_pck() {
        nfdu8char_t* path = nullptr;

        nfdu8filteritem_t filters[] = {
            { "PCK Files", "pck" }
        };

        nfdresult_t result = NFD_OpenDialogU8(
            &path,
            filters,
            1,
            nullptr
        );

        if (result == NFD_OKAY) {
            try {
                apps.open_pck(path);
            }
            catch (const std::runtime_error& e) {
                SDL_ShowSimpleMessageBox(
                    SDL_MESSAGEBOX_ERROR,
                    "Oops",
                    e.what(),
                    sdls.w
                );
            }

            NFD_FreePathU8(path);
        }
    }

    void save_pck() {
        nfdu8char_t* path = nullptr;

        nfdresult_t result = NFD_SaveDialogU8(
            &path,
            nullptr,
            0,
            nullptr,
            nullptr
        );

        if (result == NFD_OKAY) {
            try {
                apps.save_pck(path);
            }
            catch (const std::runtime_error& e) {
                SDL_ShowSimpleMessageBox(
                    SDL_MESSAGEBOX_ERROR,
                    "Oops",
                    e.what(),
                    sdls.w
                );
            }

            NFD_FreePathU8(path);
        }
    }

    void draw() {
        int ww, wh;
        SDL_GetWindowSize(sdls.w, &ww, &wh);

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {

                if (ImGui::MenuItem("Open pck"))
                    load_pck();

                if (ImGui::MenuItem("Save pck", nullptr, false, apps.pck_loaded))
                    save_pck();

                if (ImGui::MenuItem("Close pck", nullptr, false, apps.pck_loaded))
                    apps.close_pck();

                if (ImGui::MenuItem("Reload pck", nullptr, false, apps.pck_loaded))
                    apps.reload_pck();

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        float y = ImGui::GetFrameHeight();

        ImGui::SetNextWindowPos(ImVec2(0, y));
        ImGui::SetNextWindowSize(ImVec2((float)ww, (float)(wh - y)));

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("DockSpaceHost", nullptr, flags);

        ImGui::DockSpace(ImGui::GetID("MainDockSpace"));

        ImGui::End();

        assetbrowser::draw();
        assetinfo::draw();
        loadimg::draw();
    }

    void init() {
        assetbrowser::init();
        assetinfo::init();
    }

}