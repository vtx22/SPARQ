#pragma once

#include "Window.hpp"

class DebugWindow : public Window
{
public:
    DebugWindow(DataHandler& data_handler)
        : Window(ICON_FA_CODE "  Debug", data_handler)
    {
    }

    void update_content();

private:
};
