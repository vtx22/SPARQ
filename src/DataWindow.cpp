#include "DataWindow.hpp"

DataWindow::DataWindow(DataHandler *data_handler) : _data_handler(data_handler)
{
    _delete_icon.loadFromFile("./assets/icon_delete.png");
    _hide_icon.loadFromFile("./assets/icon_visibility_off.png");
    _show_icon.loadFromFile("./assets/icon_visibility_on.png");

    _hide_icon_id = gl_handle_to_imgui_id(_hide_icon.getNativeHandle());
    _show_icon_id = gl_handle_to_imgui_id(_show_icon.getNativeHandle());
    _delete_icon_id = gl_handle_to_imgui_id(_delete_icon.getNativeHandle());
}

DataWindow::~DataWindow()
{
}

void DataWindow::update()
{
    if (ImGui::Begin("Data & View"))
    {

        if (ImGui::CollapsingHeader("View"))
        {
            int selected_index = _data_handler->x_axis_select;
            if (ImGui::BeginCombo("X View", x_axis_types[selected_index].dropdown_name))
            {
                for (uint8_t n = 0; n < 3; n++)
                {
                    bool is_selected = (selected_index == n);

                    if (ImGui::Selectable(x_axis_types[n].dropdown_name, is_selected))
                    {
                        _data_handler->x_axis_select = n;
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::Checkbox("Interpolation", &_data_handler->interpolation);
            ImGui::SameLine();
            ImGui::InputInt("Subsampling Resolution", &_data_handler->ip_values_per_step, 1, 1);
            if (_data_handler->ip_values_per_step < 1)
            {
                _data_handler->ip_values_per_step = 1;
            }
            if (_data_handler->ip_values_per_step > 100)
            {
                _data_handler->ip_values_per_step = 100;
            }
        }

        if (ImGui::CollapsingHeader("Datasets"))
        {

            if (ImGui::Button("Import"))
            {
            }
            ImGui::SameLine();

            if (_data_handler->get_datasets().size() == 0)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::Button("Export"))
            {
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear All"))
            {
                _data_handler->delete_all_datasets();
            }
            else if (_data_handler->get_datasets().size() == 0)
            {
                ImGui::EndDisabled();
            }

            ImGui::Separator();

            dataset_entries(_data_handler->get_datasets_editable());
        }
    }

    ImGui::End();
}

void DataWindow::dataset_entries(std::vector<sparq_dataset_t> &datasets)
{
    if (datasets.size() == 0)
    {
        ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), "    No Data");
    }

    if (ImGui::BeginTable("##DatasetEditorTable", 6, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
    {
        std::vector<uint8_t> to_delete;
        for (uint8_t i = 0; i < datasets.size(); i++)
        {
            std::string i_str = std::to_string(i);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("  %d", datasets[i].id);
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(150);
            ImGui::InputTextWithHint(("##DsNameTB" + i_str).c_str(), "Custom Name", datasets[i].name, 64);
            ImGui::TableSetColumnIndex(2);
            ImGui::ColorEdit4(("##DsColor" + i_str).c_str(), (float *)&datasets[i].color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::TableSetColumnIndex(3);

            ImTextureID hide_icon = datasets[i].hidden ? _show_icon_id : _hide_icon_id;
            if (ImGui::ImageButton(("HIDE##" + i_str).c_str(), hide_icon, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(0.8, 0.8, 0.8, 1)))
            {
                datasets[i].toggle_visibility = true;
            }

            ImGui::TableSetColumnIndex(4);

            if (ImGui::ImageButton(("DEL##" + i_str).c_str(), _delete_icon_id, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(0.8, 0.8, 0.8, 1)))
            {
                to_delete.push_back(datasets[i].id);
            }

            ImGui::TableSetColumnIndex(5);
            ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), "%lld", datasets[i].y_values.size());
        }

        ImGui::EndTable();

        for (auto id : to_delete)
        {
            _data_handler->delete_dataset(id);
        }
    }
}

ImTextureID DataWindow::gl_handle_to_imgui_id(GLuint gl_texture_handle)
{
    ImTextureID textureID = nullptr;
    std::memcpy(&textureID, &gl_texture_handle, sizeof(GLuint));
    return textureID;
}