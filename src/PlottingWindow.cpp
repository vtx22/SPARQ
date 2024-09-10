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

            for (const auto &ds : datasets)
            {
                std::string name = (ds.name == "") ? std::to_string(ds.id) : ds.name;
                ImPlot::PlotLine((name + "###LP" + std::to_string(ds.id)).c_str(), ds.x_values.data(), ds.y_values.data(), ds.x_values.size());
            }

            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}
