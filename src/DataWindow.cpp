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

        for (uint8_t i = 0; i < datasets.size(); i++)
        {
            static char name[256];

            std::string i_str = std::to_string(i);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%d", datasets[i].id);
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(100);
            ImGui::InputTextWithHint(("##DsNameTB" + i_str).c_str(), "Custom Name", name, 256);

            datasets[i].name = std::string(name);

            ImGui::TableSetColumnIndex(2);
            ImGui::ColorEdit4(("##DsColor" + i_str).c_str(), (float *)&datasets[i].color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::TableSetColumnIndex(3);
            ImGui::Button(("DEL##" + i_str).c_str());
            ImGui::TableSetColumnIndex(4);
            ImGui::Button(("HIDE##" + i_str).c_str());
        }

        ImGui::EndTable();
    }
}