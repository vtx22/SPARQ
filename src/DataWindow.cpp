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
        dataset_entries(_data_handler->get_datasets_editable());
    }

    ImGui::End();
}

void DataWindow::dataset_entries(std::vector<sparq_dataset_t> &datasets)
{

    if (ImGui::BeginTable("##DatasetEditorTable", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
    {
        for (auto &ds : datasets)
        {
            static char name[256];
            static ImVec4 color;
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%d", ds.id);
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(100);
            ImGui::InputTextWithHint("##DatasetNameTB", "Custom Name", name, 256);

            ds.name = std::string(name);

            ImGui::TableSetColumnIndex(2);
            ImGui::ColorEdit4("##DsColor", (float *)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::TableSetColumnIndex(3);
            ImGui::Button("DEL##");
            ImGui::TableSetColumnIndex(4);
            ImGui::Button("HIDE##");
        }

        ImGui::EndTable();
    }
}