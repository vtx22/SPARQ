#include "PlottingWindow.hpp"

PlottingWindow::PlottingWindow()
{
}

PlottingWindow::~PlottingWindow()
{
}

void PlottingWindow::update(const std::vector<sparq_dataset_t> &datasets)
{
    if (ImGui::Begin("Plot"))
    {
        if (ImPlot::BeginPlot("Data", ImVec2(-1, -1)))
        {
            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3);
            ImPlot::SetupAxes("Time", "", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

            ImPlotContext *ctx = ImPlot::GetCurrentContext();
            ImPlotPlot *plot = ctx->CurrentPlot;

            uint8_t i = 0;
            for (const auto &ds : datasets)
            {
                std::string name = (ds.name[0] == 0) ? std::to_string(ds.id) : std::string(ds.name);
                ImPlot::PlotLine((name + "###LP" + std::to_string(ds.id)).c_str(), ds.x_values.data(), ds.y_values.data(), ds.x_values.size());

                ImPlotItem *item = plot->Items.GetLegendItem(i);
                item->Show = !ds.hidden;
                i++;
            }

            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}
