#pragma once

#include "Window.hpp"

namespace spq::ui
{
    class DebugWindow final : public Window
    {
    public:
        explicit DebugWindow(data::DataHandler& data_handler)
            : Window(ICON_FA_CODE "  Debug", data_handler)
        {
        }

    protected:
        void update_content(data::Datasets& datasets) override;
    };
}
