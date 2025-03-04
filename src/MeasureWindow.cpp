#include "MeasureWindow.hpp"

void MeasureWindow::update_content()
{
    auto &markers = _data_handler->get_markers();

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

void MeasureWindow::measure_markers_table(std::vector<sparq_marker_t> &markers)
{
    if (markers.size() == 0)
    {
        ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), "    No Markers");
    }

    if (ImGui::BeginTable("##MeasureMarkerTable", 7, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
    {
        std::lock_guard<std::mutex> lock(_data_handler->get_data_mutex());

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

            std::string ds_selector_name = std::to_string(markers[i].ds_id);
            if (markers[i].ds_id != -1 && datasets[markers[i].ds_index].name.length() > 0)
            {
                ds_selector_name += " [" + datasets[markers[i].ds_index].name + "]";
            }

            std::string ds_selector_preview = (datasets.size() == 0 || markers[i].ds_id == -1) ? "None Selected" : ds_selector_name;

            if (datasets.size() == 0)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::BeginCombo((std::string("###MarkerDatasetSelect") + i_str).c_str(), ds_selector_preview.c_str()))
            {
                for (uint8_t n = 0; n < datasets.size(); n++)
                {
                    bool is_selected = (n == markers[i].ds_index);

                    std::string selectable_name = std::to_string(datasets[n].id);
                    if (datasets[n].name.length() > 0)
                    {
                        selectable_name += " [" + datasets[n].name + "]";
                    }

                    if (ImGui::Selectable(selectable_name.c_str(), is_selected))
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

            std::string hide_text = std::string(markers[i].hidden ? ICON_FA_EYE_SLASH : ICON_FA_EYE) + "##HIDE" + i_str;
            if (ImGui::Button(hide_text.c_str()))
            {
                markers[i].hidden = !markers[i].hidden;
            }

            ImGui::TableSetColumnIndex(4);

            std::string del_text = std::string(ICON_FA_TRASH) + "##DEL" + i_str;
            if (ImGui::Button(del_text.c_str()))
            {
                to_delete.push_back(i);
            }

            ImGui::TableSetColumnIndex(5);
            ImGui::Text((std::string("x: ") + std::to_string(markers[i].x)).c_str());

            ImGui::TableSetColumnIndex(6);
            ImGui::Text((std::string("y: ") + std::to_string(markers[i].y)).c_str());
        }

        ImGui::EndTable();

        std::sort(to_delete.rbegin(), to_delete.rend());
        for (auto id : to_delete)
        {
            markers.erase(markers.begin() + id);
        }
    }
}