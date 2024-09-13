#include "DataWindow.hpp"

DataWindow::DataWindow(DataHandler *data_handler) : _data_handler(data_handler)
{
}

DataWindow::~DataWindow()
{
}

void DataWindow::update()
{
    if (ImGui::Begin("Data"))
    {
        ImGui::SeparatorText("Datasets");

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

        ImGui::Separator();

        if (_data_handler->get_datasets().size() == 0)
        {
            ImGui::EndDisabled();
        }

        dataset_entries(_data_handler->get_datasets_editable());
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

            if (ImGui::Button(("DEL##" + i_str).c_str()))
            {
                to_delete.push_back(datasets[i].id);
            }

            ImGui::TableSetColumnIndex(4);

            if (ImGui::Button(("HIDE##" + i_str).c_str()))
            {
                datasets[i].toggle_visibility = true;
            }
        }

        ImGui::EndTable();

        for (auto id : to_delete)
        {
            _data_handler->delete_dataset(id);
        }
    }
}