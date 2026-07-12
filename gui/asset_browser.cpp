#include "asset_browser.h"

#include "state.h"

#include <imgui.h>

void assetbrowser::draw() {
    ImGui::Begin("Asset Browser");

    constexpr float thumbnail_size = 96.0f;
    constexpr float padding = 8.0f;

    float avail = ImGui::GetContentRegionAvail().x;
    int columns = std::max(1, int(avail / (thumbnail_size + padding)));

    if (ImGui::BeginTable("Assets", columns,
                          ImGuiTableFlags_SizingStretchSame))
    {
        for (size_t i = 0; i < apps.images.size(); ++i) {
            ImGui::TableNextColumn();

            constexpr float thumb = 96.0f;

            ImGui::PushID((int)i);

            ImVec2 p = ImGui::GetCursorScreenPos();

            bool clicked = ImGui::InvisibleButton("thumb", ImVec2(thumb, thumb));

            ImDrawList* dl = ImGui::GetWindowDrawList();

            ImU32 bg = ImGui::GetColorU32(
                ImGui::IsItemHovered() ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            dl->AddRectFilled(p, ImVec2(p.x + thumb, p.y + thumb), bg);
            
            if (apps.current_selected == (int)i)
            {
                dl->AddRect(
                    p,
                    ImVec2(p.x + thumb, p.y + thumb),
                    IM_COL32(255, 255, 0, 255),
                    0.0f,
                    0,
                    2.0f);
            }

            // Compute centered image size
            const auto& img = apps.images[i].image.image;

            float scale = std::min(
                thumb / img.width,
                thumb / img.height);
            
            ImVec2 size(
                img.width * scale,
                img.height * scale);
            
            ImVec2 pos(
                p.x + (thumb - size.x) * 0.5f,
                p.y + (thumb - size.y) * 0.5f);
            
            // Draw image
            dl->AddImage(
                (ImTextureID)(intptr_t)apps.images[i].gl_texture,
                pos,
                ImVec2(pos.x + size.x, pos.y + size.y));
            
            if (clicked)
                apps.current_selected = (int)i;
            
            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::End();
}
void assetbrowser::init() {}