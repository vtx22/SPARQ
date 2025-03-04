#include "StatisticsWindow.hpp"

void StatisticsWindow::update_content()
{
    ImGui::Text("FPS: %d", (int)round(ImGui::GetIO().Framerate));
}