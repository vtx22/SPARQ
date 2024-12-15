#include "DataWindow.hpp"

DataWindow::DataWindow(DataHandler *data_handler) : _data_handler(data_handler)
{
}

DataWindow::~DataWindow()
{
}

void DataWindow::update()
{
    if (ImGui::Begin(ICON_FA_DATABASE "  Data & View"))
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
            ImGui::InputTextWithHint(("##DsNameTB" + i_str).c_str(), "Custom Name", datasets[i].name_buffer, 64);
            datasets[i].name = datasets[i].name_buffer;
            ImGui::TableSetColumnIndex(2);
            ImGui::ColorEdit4(("##DsColor" + i_str).c_str(), (float *)&datasets[i].color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::TableSetColumnIndex(3);

            std::string hide_text = std::string(datasets[i].hidden ? ICON_FA_EYE_SLASH : ICON_FA_EYE) + "##HIDE" + i_str;
            if (ImGui::Button(hide_text.c_str()))
            {
                datasets[i].toggle_visibility = true;
            }

            ImGui::TableSetColumnIndex(4);
            std::string clear_text = std::string(ICON_FA_SQUARE_XMARK) + "##CLEAR" + i_str;
            if (ImGui::Button(clear_text.c_str()))
            {
                _data_handler->clear_dataset(datasets[i].id);
            }

            ImGui::TableSetColumnIndex(5);

            std::string del_text = std::string(ICON_FA_TRASH) + "##DEL" + i_str;
            if (ImGui::Button(del_text.c_str()))
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