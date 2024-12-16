#include "DataWindow.hpp"

DataWindow::DataWindow(DataHandler *data_handler) : _data_handler(data_handler)
{
}

DataWindow::~DataWindow()
{
}

void DataWindow::update()
{
    if (ImGui::Begin(ICON_FA_DATABASE "  Data"))
    {
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

    if (ImGui::BeginTable("##DatasetEditorTable", 8, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
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
            std::string wave_type_text = std::string(datasets[i].display_square ? ICON_FA_WAVE_SQUARE : ICON_FA_MINUS) + "##WAVE" + i_str;
            if (ImGui::Button(wave_type_text.c_str()))
            {
                datasets[i].display_square = !datasets[i].display_square;
            }

            ImGui::TableSetColumnIndex(5);
            std::string clear_text = std::string(ICON_FA_SQUARE_XMARK) + "##CLEAR" + i_str;
            if (ImGui::Button(clear_text.c_str()))
            {
                _data_handler->clear_dataset(datasets[i].id);
            }

            ImGui::TableSetColumnIndex(6);

            std::string del_text = std::string(ICON_FA_TRASH) + "##DEL" + i_str;
            if (ImGui::Button(del_text.c_str()))
            {
                to_delete.push_back(datasets[i].id);
            }

            ImGui::TableSetColumnIndex(7);
            ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), "%lld", datasets[i].y_values.size());
        }

        ImGui::EndTable();

        for (auto id : to_delete)
        {
            _data_handler->delete_dataset(id);
        }
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