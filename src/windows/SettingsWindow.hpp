#pragma once

#include "Window.hpp"

namespace spq::ui
{
    class SettingsWindow final : public Window
    {
    public:
        explicit SettingsWindow(data::DataHandler& data_handler);

        void show_downsampling_settings();
        void show_graphics_settings();
        void show_color_settings();

    private:
        bool _settings_changed = false;

    protected:
        void update_content(data::Datasets& datasets) override;
    };
}
