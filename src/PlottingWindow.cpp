#include "PlottingWindow.hpp"

PlottingWindow::PlottingWindow()
{
}

PlottingWindow::~PlottingWindow()
{
}

void PlottingWindow::update(std::vector<sparq_dataset_t> &datasets)
{
    if (ImGui::Begin("Plot"))
    {
        if (ImPlot::BeginPlot("Data", ImVec2(-1, -1)))
        {
            ImPlot::SetupAxes("Time", "", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

            ImPlotContext *ctx = ImPlot::GetCurrentContext();
            ImPlotPlot *plot = ctx->CurrentPlot;

            uint8_t i = 0;
            for (auto &ds : datasets)
            {
                std::string name = (ds.name[0] == 0) ? std::to_string(ds.id) : std::string(ds.name);
                ImPlot::SetNextLineStyle(ds.color, 3);
                ImPlot::PlotLine((name + "###LP" + std::to_string(ds.id)).c_str(), ds.x_values.data(), ds.y_values.data(), ds.x_values.size());

                ImPlotItem *item = plot->Items.GetLegendItem(i);
                if (ds.toggle_visibility)
                {
                    item->Show = !item->Show;
                    ds.toggle_visibility = false;
                }

                i++;
            }

            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}
