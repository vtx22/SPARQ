#pragma once

#include <sstream>
#include <string>

#include "Window.hpp"

class SettingsWindow : public Window
{
public:
    SettingsWindow(DataHandler *data_handler);

    void update_content() override;

    void show_downsampling_settings();
    void show_graphics_settings();

private:
    bool _settings_changed = false;
};