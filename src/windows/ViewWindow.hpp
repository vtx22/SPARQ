#pragma once

#include "../sparq_types.hpp"
#include "Window.hpp"
#include "imgui-SFML.h"
#include "implot.h"
#include "plotting/PlotSettings.hpp"

namespace spq::ui
{
    class ViewWindow final : public Window
    {
    public:
        using PlotData = std::pair<std::unordered_set<std::size_t>&, plotting::plot_settings&>;
        using CreatePlottingWindowCallback = std::function<void(plotting::plot_type)>;
        using GetSelectedPlotDataCallback = std::function<std::optional<PlotData>()>;

        ViewWindow(
            data::DataHandler& data_handler,
            CreatePlottingWindowCallback on_create_plotting_window,
            GetSelectedPlotDataCallback get_selected_plot_data)
            : Window(ICON_FA_SLIDERS "  View", data_handler),
              m_on_create_plotting_window(std::move(on_create_plotting_window)),
              m_get_selected_plot_data(std::move(get_selected_plot_data))
        {
            SPQ_ASSERT(m_on_create_plotting_window, "_on_create_plotting_window wasn't a callable!");
            SPQ_ASSERT(m_get_selected_plot_data, "_get_selected_plot_settings wasn't a callable!");
        }

    private:
        CreatePlottingWindowCallback m_on_create_plotting_window;
        GetSelectedPlotDataCallback m_get_selected_plot_data;

        static void show_dataset_selection(std::unordered_set<std::size_t>& ids_to_plot, data::Datasets& datasets);

    protected:
        void update_content(data::Datasets& datasets) override;
    };
}
