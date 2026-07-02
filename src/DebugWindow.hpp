#pragma once

#include "Window.hpp"

class DebugWindow final : public Window
{
public:
    DebugWindow(DataHandler& data_handler)
        : Window(ICON_FA_CODE "  Debug", data_handler)
    {
    }

protected:
    void update_content() override;

private:
};
