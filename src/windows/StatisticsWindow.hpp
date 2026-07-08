#pragma once

#include "Window.hpp"

namespace spq::ui
{
    class StatisticsWindow final : public Window
    {
    public:
        explicit StatisticsWindow(DataHandler& data_handler)
            : Window(ICON_FA_MAGNIFYING_GLASS_CHART "  Statistics", data_handler)
        {
        }

    protected:
        void update_content(Datasets& datasets) override
        {
            ImGui::Text("FPS: %d", static_cast<int>(round(ImGui::GetIO().Framerate)));
        }
    };
}
