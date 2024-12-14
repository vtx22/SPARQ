#include "MeasureWindow.hpp"

MeasureWindow::MeasureWindow(AssetHolder *asset_holder, DataHandler *data_handler) : _asset_holder(asset_holder), _data_handler(data_handler)
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
    auto &markers = _data_handler->get_markers();

    if (ImGui::Begin("Measure"))
    {
        if (ImGui::CollapsingHeader("Markers"))
        {
            if (ImGui::Button("Add"))
            {
                sparq_marker_t marker;
                marker.name = std::string("M") + std::to_string(markers.size());
                markers.push_back(marker);
            }

            ImGui::SameLine();

            if (ImGui::Button("Hide All"))
            {
                for (auto &m : markers)
                {
                    m.hidden = true;
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Delete All"))
            {
                markers.clear();
            }

            ImGui::Separator();

            measure_markers_table(markers);
        }
    }
    ImGui::End();
}

void MeasureWindow::measure_markers_table(std::vector<sparq_marker_t> &markers)
{
    if (markers.size() == 0)
    {
        ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), "    No Markers");
    }

    if (ImGui::BeginTable("##MeasureMarkerTable", 6, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
    {
        std::vector<uint32_t> to_delete;
        for (size_t i = 0; i < markers.size(); i++)
        {
            std::string i_str = std::to_string(i);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text(markers[i].name.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(200);

            auto &datasets = _data_handler->get_datasets();

            const char *ds_selector_preview = (datasets.size() == 0 || markers[i].ds_id == -1) ? "None Selected" : std::to_string(markers[i].ds_id).c_str();

            if (datasets.size() == 0)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::BeginCombo((std::string("###MarkerDatasetSelect") + i_str).c_str(), ds_selector_preview))
            {
                for (uint8_t n = 0; n < datasets.size(); n++)
                {
                    bool is_selected = (n == markers[i].ds_index);

                    if (ImGui::Selectable(std::to_string(datasets[n].id).c_str(), is_selected))
                    {
                        markers[i].ds_index = n;
                        markers[i].ds_id = datasets[n].id;
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (datasets.size() == 0)
            {
                ImGui::EndDisabled();
            }

            ImGui::TableSetColumnIndex(2);
            ImGui::ColorEdit4(("##DsColor" + i_str).c_str(), (float *)&markers[i].color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::TableSetColumnIndex(3);

            ImTextureID hide_icon = markers[i].hidden ? _show_icon_id : _hide_icon_id;
            if (ImGui::ImageButton(("HIDE##" + i_str).c_str(), hide_icon, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(0.8, 0.8, 0.8, 1)))
            {
                markers[i].hidden = !markers[i].hidden;
            }

            ImGui::TableSetColumnIndex(4);

            if (ImGui::ImageButton(("DEL##" + i_str).c_str(), _delete_icon_id, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(0.8, 0.8, 0.8, 1)))
            {
                to_delete.push_back(i);
            }

            ImGui::TableSetColumnIndex(5);
            ImGui::Text((std::string("x: ") + std::to_string(markers[i].x)).c_str());
        }

        ImGui::EndTable();

        std::sort(to_delete.rbegin(), to_delete.rend());
        for (auto id : to_delete)
        {
            markers.erase(markers.begin() + id);
        }
    }
}