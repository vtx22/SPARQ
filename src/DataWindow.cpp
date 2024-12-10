#include "DataWindow.hpp"

DataWindow::DataWindow(DataHandler *data_handler, AssetHolder *asset_holder) : _data_handler(data_handler), _asset_holder(asset_holder)
{
    _hide_icon_id = _asset_holder->add_asset("./assets/icon_visibility_off.png");
    _show_icon_id = _asset_holder->add_asset("./assets/icon_visibility_on.png");
    _delete_icon_id = _asset_holder->add_asset("./assets/icon_delete.png");
    _clear_data_icon_id = _asset_holder->add_asset("./assets/icon_clear_data.png");
}

DataWindow::~DataWindow()
{
}

void DataWindow::update()
{
    if (ImGui::Begin("Data & View"))
    {
        show_view_section();
        show_datasets_section();
    }

    ImGui::End();
}

void DataWindow::dataset_entries(std::vector<sparq_dataset_t> &datasets)
{
    if (datasets.size() == 0)
    {
        ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), "    No Data");
    }

    if (ImGui::BeginTable("##DatasetEditorTable", 7, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
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
            if (ImGui::ImageButton(("CLEAR##" + i_str).c_str(), _clear_data_icon_id, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(0.8, 0.8, 0.8, 1)))
            {
                _data_handler->clear_dataset(datasets[i].id);
            }

            ImGui::TableSetColumnIndex(5);

            if (ImGui::ImageButton(("DEL##" + i_str).c_str(), _delete_icon_id, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(0.8, 0.8, 0.8, 1)))
            {
                to_delete.push_back(datasets[i].id);
            }

            ImGui::TableSetColumnIndex(6);
            ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), "%lld", datasets[i].y_values.size());
        }

        ImGui::EndTable();

        for (auto id : to_delete)
        {
            _data_handler->delete_dataset(id);
        }
    }
}

void DataWindow::show_view_section()
{
    int text_offset = 90;
    if (ImGui::CollapsingHeader("View"))
    {
        ImGui::SeparatorText("X Axis");

        ImGui::Text("Axis Unit");
        ImGui::SameLine();
        ImGui::SetCursorPosX(text_offset);
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::BeginCombo("##X View", x_axis_types[_data_handler->x_axis_select].dropdown_name))
        {
            for (uint8_t n = 0; n < x_axis_types.size(); n++)
            {
                bool is_selected = (_data_handler->x_axis_select == n);

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

        ImGui::Text("Fit");
        ImGui::SameLine();
        ImGui::SetCursorPosX(text_offset);
        ImGui::SetNextItemWidth(-200);
        if (ImGui::BeginCombo("##X Fit", x_axis_fits[_data_handler->x_fit_select]))
        {
            for (uint8_t n = 0; n < x_axis_fits.size(); n++)
            {
                bool is_selected = (_data_handler->x_fit_select == n);

                if (ImGui::Selectable(x_axis_fits[n], is_selected))
                {
                    _data_handler->x_fit_select = n;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine();

        if (_data_handler->x_fit_select != 2)
        {
            ImGui::BeginDisabled();
        }

        ImGui::SetNextItemWidth(-210);
        ImGui::Text("N =");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(160);
        ImGui::InputInt("##LastN", &_data_handler->last_n, 1, 10);
        if (_data_handler->last_n < 2)
        {
            _data_handler->last_n = 2;
        }

        if (_data_handler->x_fit_select != 2)
        {
            ImGui::EndDisabled();
        }

        ImGui::SeparatorText("Y Axis");
        ImGui::Text("Fit");
        ImGui::SameLine();
        ImGui::SetCursorPosX(text_offset);
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::BeginCombo("##Y Fit", y_axis_fits[_data_handler->y_fit_select]))
        {
            for (uint8_t n = 0; n < y_axis_fits.size(); n++)
            {
                bool is_selected = (_data_handler->y_fit_select == n);

                if (ImGui::Selectable(y_axis_fits[n], is_selected))
                {
                    _data_handler->y_fit_select = n;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::SeparatorText("Other");
        ImGui::BeginDisabled();
        ImGui::Checkbox("Interpolation", &_data_handler->interpolation);
        ImGui::EndDisabled();
    }
}

void DataWindow::show_datasets_section()
{
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
            _data_handler->export_data_csv();
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear All"))
        {
            _data_handler->clear_all_datasets();
        }

        ImGui::SameLine();

        if (ImGui::Button("Delete All"))
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