#include "main_window.h"
#include <SDL3/SDL.h>
#include "state.h"
#include "asset_browser.h"
#include <imgui.h>
#include "loadimg.h"

#include "asset_browser.h"
#include "asset_info.h"

namespace mainwindow {
    void pck_selected_cb(void *userdata, const char *const *filelist, int filter) {
        if (filelist == NULL) {
            SDL_Log("An error occurred opening the file dialog: %s", SDL_GetError());
            return;
        }

        if (*filelist == NULL) {
            SDL_Log("User canceled the selection.");
            return;
        }

        apps.open_pck(std::string(filelist[0]));
    }

    void save_pck_cb(void *userdata, const char *const *filelist, int filter) {
        if (filelist == NULL) {
            return;
        }
        if(*filelist == NULL) {
            return;
        }
        SDL_PathInfo info;
        try {
            apps.save_pck(std::string(filelist[0]));
        } catch (std::runtime_error e) {
            SDL_ShowSimpleMessageBox(
                SDL_MESSAGEBOX_ERROR,
                "Oops",
                "Cant save file!",
                sdls.w
            );
        }
    }

    void save_pck() {
        const SDL_DialogFileFilter filters[] = {
            { "Pck Files (*.pck)", "pck" },
        };
        SDL_ShowSaveFileDialog(save_pck_cb, NULL, sdls.w, filters, 1, "./");
    }

    void load_pck() {
        const SDL_DialogFileFilter filters[] = {
            { "Pck Files (*.pck)", "pck" },
        };
        SDL_ShowOpenFileDialog(
            pck_selected_cb,
            NULL,
            sdls.w,
            filters,
            1,
            "./",
            false
        );
    }

    void draw() {
        int ww, wh;
        SDL_GetWindowSize(sdls.w, &ww, &wh);

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Open pck"))
                    load_pck();

                if(ImGui::MenuItem("Save pck", NULL, false, apps.pck_loaded))
                    save_pck();
                if(ImGui::MenuItem("Close pck", NULL, false, apps.pck_loaded))
                    apps.close_pck();
                if(ImGui::MenuItem("Reload pck", NULL, false, apps.pck_loaded))
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

        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspace_id);
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