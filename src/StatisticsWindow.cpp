#include "StatisticsWindow.hpp"

StatisticsWindow::StatisticsWindow()
{
}

StatisticsWindow::~StatisticsWindow()
{
}

void StatisticsWindow::update()
{
    if (ImGui::Begin(ICON_FA_MAGNIFYING_GLASS_CHART "  Statistics"))
    {
        ImGui::Text("FPS: %d", (int)round(ImGui::GetIO().Framerate));
    }

    ImGui::End();
}