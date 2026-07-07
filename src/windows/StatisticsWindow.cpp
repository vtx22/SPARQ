#include "../StatisticsWindow.hpp"

void StatisticsWindow::update_content(Datasets& datasets)
{
    ImGui::Text("FPS: %d", static_cast<int>(round(ImGui::GetIO().Framerate)));
}
