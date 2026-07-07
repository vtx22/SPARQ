#include "DataWindow.hpp"

void DataWindow::update_content(Datasets& datasets)
{
    show_datasets_section(datasets);
}

void DataWindow::dataset_entries(Datasets& datasets) const
{
    if (datasets.empty())
    {
        ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), "    No Data");
    }

    if (ImGui::BeginTable("##DatasetEditorTable", 7, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
    {
        std::vector<uint8_t> to_delete;
        for (std::size_t i = 0; i < datasets.size(); i++)
        {
            std::string const i_str = std::to_string(i);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("  %d", datasets[i].id);
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(150);
            char* name_buffer = datasets[i].name_buffer;
            ImGui::InputTextWithHint(
                ("##DsNameTB" + i_str).c_str(),
                "Custom Name",
                name_buffer,
                sizeof(name_buffer));
            datasets[i].name = name_buffer;
            ImGui::TableSetColumnIndex(2);
            ImGui::ColorEdit4(
                ("##DsColor" + i_str).c_str(),
                reinterpret_cast<float*>(&datasets[i].color),
                ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

            ImGui::TableSetColumnIndex(3);

            std::string const wave_type_text = std::string(datasets[i].display_square ? ICON_FA_WAVE_SQUARE : ICON_FA_MINUS)
                                             + "##WAVE" + i_str;
            if (ImGui::Button(wave_type_text.c_str()))
            {
                datasets[i].display_square = !datasets[i].display_square;
            }

            ImGui::TableSetColumnIndex(4);

            std::string const clear_text = std::string(ICON_FA_SQUARE_XMARK)
                                         + "##CLEAR" + i_str;
            if (ImGui::Button(clear_text.c_str()))
            {
                datasets.clear(datasets[i].id);
            }

            ImGui::TableSetColumnIndex(5);

            std::string const del_text = std::string(ICON_FA_TRASH)
                                       + "##DEL" + i_str;
            if (ImGui::Button(del_text.c_str()))
            {
                to_delete.push_back(datasets[i].id);
            }

            ImGui::TableSetColumnIndex(6);
            ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), "%zu", datasets[i].y_values.size());
        }

        ImGui::EndTable();

        for (auto const id : to_delete)
        {
            datasets.delete_dataset(id);
        }
    }
}

void DataWindow::show_datasets_section(Datasets& datasets)
{
    if (ImGui::CollapsingHeader("Serial Datasets"))
    {
        if (ImGui::Button("Import"))
        {
        }
        ImGui::SameLine();

        if (datasets.empty())
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Export"))
        {
            DataHandler::export_datasets_csv(datasets);
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear All"))
        {
            datasets.clear_all();
        }

        ImGui::SameLine();

        bool just_deleted = false;
        if (ImGui::Button("Delete All"))
        {
            datasets.delete_all();
            just_deleted = true;
        }

        ImGui::SameLine();

        if (!just_deleted && datasets.empty())
        {
            ImGui::EndDisabled();
        }

        ImGui::Separator();

        dataset_entries(datasets);
    }

    if (ImGui::CollapsingHeader("Synthetic Datasets"))
    {
    }

    if (ImGui::CollapsingHeader("Data Manipulators"))
    {
    }

    if (ImGui::CollapsingHeader("Math Functions"))
    {
    }
}
