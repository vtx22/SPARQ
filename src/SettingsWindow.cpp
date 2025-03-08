#include "SettingsWindow.hpp"

SettingsWindow::SettingsWindow(DataHandler *data_handler) : Window(ICON_FA_GEAR "  Settings", data_handler)
{
}

void SettingsWindow::update_content()
{
    show_graphics_settings();
    show_downsampling_settings();
    show_color_settings();

    if (_settings_changed)
    {
        _config_handler.write_config();
        _settings_changed = false;
    }
}

void SettingsWindow::show_downsampling_settings()
{
    if (ImGui::CollapsingHeader("Downsampling"))
    {
        bool downsampling_enabled = _config_handler.ini["downsampling"]["enabled"] == "1";
        if (ImGui::Checkbox("Enabled", &downsampling_enabled))
        {
            _config_handler.ini["downsampling"]["enabled"] = downsampling_enabled ? "1" : "0";
            _settings_changed = true;
        }

        ImGui::Text("Max Samples:");
        ImGui::SameLine();

        int max_samples = std::stoi(_config_handler.ini["downsampling"]["max_samples"]);

        float spacing_right = 7.f * ImGui::GetFontSize();
        ImGui::SetNextItemWidth(-spacing_right);

        if (ImGui::InputInt("##MaxSamples", &max_samples, 0, 0))
        {
            if (max_samples < 10)
            {
                max_samples = 10;
            }

            _config_handler.ini["downsampling"]["max_samples"] = std::to_string(max_samples);
            _settings_changed = true;
        }

        ImGui::SameLine();

        ImGui::SetNextItemWidth(spacing_right);

        const char *max_samples_types[2] = {"Total", "Per Dataset"};
        int max_samples_type = std::stoi(_config_handler.ini["downsampling"]["max_samples_type"]);
        int prev_max_samples_type = max_samples_type;
        if (ImGui::BeginCombo("##X View", max_samples_types[max_samples_type]))
        {
            for (uint8_t n = 0; n < 2; n++)
            {
                bool is_selected = (max_samples_type == n);

                if (ImGui::Selectable(max_samples_types[n], is_selected))
                {
                    max_samples_type = n;
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (max_samples_type != prev_max_samples_type)
        {
            _config_handler.ini["downsampling"]["max_samples_type"] = std::to_string(max_samples_type);
            _settings_changed = true;
        }
    }
}

void SettingsWindow::show_graphics_settings()
{
    if (ImGui::CollapsingHeader("Graphics"))
    {
        bool vsync_enabled = _config_handler.ini["graphics"]["vsync"] == "1";

        if (ImGui::Checkbox("VSYNC", &vsync_enabled))
        {
            _config_handler.ini["graphics"]["vsync"] = vsync_enabled ? "1" : "0";
            _settings_changed = true;
        }

        ImGui::Text("Antialiasing Level:");
        ImGui::SameLine();

        int antialiasing_level = std::stoi(_config_handler.ini["graphics"]["antialiasing"]);

        if (ImGui::InputInt("##MaxSamples", &antialiasing_level, 0, 0))
        {
            if (antialiasing_level < 0)
            {
                antialiasing_level = 0;
            }
            if (antialiasing_level > 8)
            {
                antialiasing_level = 8;
            }

            _config_handler.ini["graphics"]["antialiasing"] = std::to_string(antialiasing_level);
            _settings_changed = true;
        }
    }
}

void SettingsWindow::show_color_settings()
{
    if (ImGui::CollapsingHeader("Color"))
    {

        ImGui::Text("Color Map:");
        ImGui::SameLine();

        ImPlotColormap cm_prev = ImPlot::GetStyle().Colormap;
        ImPlot::ShowColormapSelector("##Colormap");

        if (cm_prev != ImPlot::GetStyle().Colormap)
        {
            _config_handler.ini["color"]["colormap"] = std::to_string(ImPlot::GetStyle().Colormap);
            _settings_changed = true;
        }
    }
}