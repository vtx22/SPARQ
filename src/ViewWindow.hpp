#pragma once

#include "Window.hpp"
#include "imgui-SFML.h"
#include "implot.h"
#include "sparq_types.hpp"

class ViewWindow : public Window
{
public:
    using CreatePlottingWindowCallback = std::function<void()>;
    using GetSelectedPlotSettingsCallback = std::function<std::optional<std::reference_wrapper<spq::plotting::plot_settings>>()>;

    ViewWindow(
        DataHandler& data_handler,
        CreatePlottingWindowCallback on_create_plotting_window,
        GetSelectedPlotSettingsCallback get_selected_plot_settings)
        : Window(ICON_FA_SLIDERS "  View", data_handler),
          _on_create_plotting_window(std::move(on_create_plotting_window)),
          _get_selected_plot_settings(std::move(get_selected_plot_settings))
    {
    }

    void update_content() override;

private:
    void show_plot_settings(spq::plotting::plot_settings& settings);

    CreatePlottingWindowCallback _on_create_plotting_window;
    GetSelectedPlotSettingsCallback _get_selected_plot_settings;
};
