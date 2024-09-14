#include "DataWindow.hpp"

DataWindow::DataWindow(DataHandler *data_handler) : _data_handler(data_handler)
{
    _delete_icon.loadFromFile("./assets/icon_delete.png");
    _hide_icon.loadFromFile("./assets/icon_visibility_off.png");
    _show_icon.loadFromFile("./assets/icon_visibility_on.png");

    _hide_icon_id = gl_handle_to_imgui_id(_hide_icon.getNativeHandle());
    _show_icon_id = gl_handle_to_imgui_id(_show_icon.getNativeHandle());
}

DataWindow::~DataWindow()
{
}

void DataWindow::update()
{
    if (ImGui::Begin("Data & View"))
    {
        const char *x_axis_types[3] = {"Samples", "Relative Time", "Absolute Time"};

        ImGui::SeparatorText("View Settings");

        static int selected_axis_type = 0;
        if (ImGui::BeginCombo("X View", x_axis_types[selected_axis_type]))
        {
            for (uint8_t n = 0; n < 3; n++)
            {
                bool is_selected = (selected_axis_type == n);

                if (ImGui::Selectable(x_axis_types[n], is_selected))
                {
                    selected_axis_type = n;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SeparatorText("Data");

        if (_data_handler->get_datasets().size() == 0)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Import"))
        {
        }
        ImGui::SameLine();
        if (ImGui::Button("Export"))
        {
        }

        if (_data_handler->get_datasets().size() == 0)
        {
            ImGui::EndDisabled();
        }

        if (ImGui::CollapsingHeader("Datasets", ImGuiTreeNodeFlags_DefaultOpen))
        {
            dataset_entries(_data_handler->get_datasets_editable());
        }
    }

    ImGui::End();
}

void DataWindow::dataset_entries(std::vector<sparq_dataset_t> &datasets)
{
    if (datasets.size() == 0)
    {
        ImGui::Text("No Datasets present.");
    }

    if (ImGui::BeginTable("##DatasetEditorTable", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
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

            if (ImGui::ImageButton(("DEL##" + i_str).c_str(), gl_handle_to_imgui_id(_delete_icon.getNativeHandle()), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(0.8, 0.8, 0.8, 1)))
            {
                to_delete.push_back(datasets[i].id);
            }
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