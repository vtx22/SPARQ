#pragma once

#include <sstream>
#include <string>

#include "Window.hpp"

class SettingsWindow : public Window
{
public:
    SettingsWindow(DataHandler *data_handler);

    void update_content() override;
};