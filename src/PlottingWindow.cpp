#include "PlottingWindow.hpp"

PlottingWindow::PlottingWindow()
{
}

PlottingWindow::~PlottingWindow()
{
}

void PlottingWindow::update()
{
    if (ImGui::Begin("Plot"))
    {
        if (ImPlot::BeginPlot("Data", ImVec2(-1, -1)))
        {
            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3);
            ImPlot::SetupAxes("Time", "");
            // ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

            // for (auto &ds : data)
            //{
            //     ImPlot::PlotLine(std::to_string(ds.uuid).c_str(), ds.x_values.data(), ds.y_values.data(), 255);
            // }

            // ImPlot::PopStyleColor();
            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}