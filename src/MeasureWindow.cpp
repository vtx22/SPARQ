#include "MeasureWindow.hpp"

MeasureWindow::MeasureWindow(AssetHolder *asset_holder) : _asset_holder(asset_holder)
{
    _show_icon_id = _asset_holder->get_handle("icon_visibility_on.png");
    _hide_icon_id = _asset_holder->get_handle("icon_visibility_off.png");
    _delete_icon_id = _asset_holder->get_handle("icon_delete.png");
}

MeasureWindow::~MeasureWindow()
{
}

void MeasureWindow::update()
{
    if (ImGui::Begin("Measure"))
    {
        if (ImGui::CollapsingHeader("Markers"))
        {
            if (ImGui::Button("Add"))
            {
                sparq_marker_t marker;
                _markers.push_back(marker);
            }

            ImGui::SameLine();

            if (ImGui::Button("Hide All"))
            {
            }

            ImGui::SameLine();

            if (ImGui::Button("Delete All"))
            {
            }

            ImGui::Separator();

            measure_markers();
        }
    }
    ImGui::End();
}

void MeasureWindow::measure_markers()
{
    if (_markers.size() == 0)
    {
        ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), "    No Markers");
    }

    if (ImGui::BeginTable("##MeasureMarkerTable", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
    {
        std::vector<uint32_t> to_delete;
        for (size_t i = 0; i < _markers.size(); i++)
        {
            std::string i_str = std::to_string(i);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("M");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(150);
            ImGui::Text("Selector");
            ImGui::TableSetColumnIndex(2);
            ImGui::ColorEdit4(("##DsColor" + i_str).c_str(), (float *)&_markers[i].color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::TableSetColumnIndex(3);

            ImTextureID hide_icon = _markers[i].hidden ? _show_icon_id : _hide_icon_id;
            if (ImGui::ImageButton(("HIDE##" + i_str).c_str(), hide_icon, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(0.8, 0.8, 0.8, 1)))
            {
                _markers[i].hidden != _markers[i].hidden;
            }

            ImGui::TableSetColumnIndex(4);

            if (ImGui::ImageButton(("DEL##" + i_str).c_str(), _delete_icon_id, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(0.8, 0.8, 0.8, 1)))
            {
                to_delete.push_back(i);
            }
        }

        ImGui::EndTable();

        for (auto id : to_delete)
        {
        }
    }
}