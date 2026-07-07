#pragma once

#include "../sparq_types.hpp"
#include "Window.hpp"
#include "imgui-SFML.h"
#include "implot.h"

class ViewWindow final : public Window
{
public:
    using CreatePlottingWindowCallback = std::function<void()>;
    using GetSelectedPlotSettingsCallback = std::function<std::optional<std::reference_wrapper<spq::plotting::plot_settings_t>>()>;

    ViewWindow(
        DataHandler& data_handler,
        CreatePlottingWindowCallback on_create_plotting_window,
        GetSelectedPlotSettingsCallback get_selected_plot_settings)
        : Window(ICON_FA_SLIDERS "  View", data_handler),
          _on_create_plotting_window(std::move(on_create_plotting_window)),
          _get_selected_plot_settings(std::move(get_selected_plot_settings))
    {
        SPQ_ASSERT(_on_create_plotting_window, "_on_create_plotting_window wasn't a callable!");
        SPQ_ASSERT(_get_selected_plot_settings, "_get_selected_plot_settings wasn't a callable!");
    }

private:
    void show_plot_settings(spq::plotting::plot_settings_t& settings);
    void show_heatmap_settings(spq::plotting::heatmap_settings_t& settings);
    void show_axis_settings(spq::plotting::plot_settings_t& settings);

    CreatePlottingWindowCallback _on_create_plotting_window;
    GetSelectedPlotSettingsCallback _get_selected_plot_settings;

protected:
    void update_content(Datasets& datasets) override;
};
