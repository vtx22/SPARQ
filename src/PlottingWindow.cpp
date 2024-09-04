#include "PlottingWindow.hpp"

PlottingWindow::PlottingWindow()
{
}

PlottingWindow::~PlottingWindow()
{
}

void PlottingWindow::add_dataset(const sparq_dataset &dataset)
{
    _datasets.push_back(dataset);
}

void PlottingWindow::update()
{
    if (ImGui::Begin("Plot"))
    {
        if (ImPlot::BeginPlot("Data", ImVec2(-1, -1)))
        {
            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3);
            ImPlot::SetupAxes("Time", "");

            for (auto &ds : _datasets)
            {
                ImPlot::PlotLine(std::to_string(ds.id).c_str(), ds.x_values.data(), ds.y_values.data(), ds.x_values.size());
            }

            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}