#pragma once

#include "pch.hpp"

#include "IconsFontAwesome6.h"
#include "ImGuiNotify.hpp"
#include "fa-solid-900.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "imgui-SFML.h"
#include "imgui.h"
#include "implot.h"

#include "imstyles.hpp"
#include "serial.hpp"

#include "sparq_config.h"
#include "sparq_types.hpp"

#include "ConfigHandler.hpp"

#include "DataHandler.hpp"

#include "windows/ConnectionWindow.hpp"
#include "windows/ConsoleWindow.hpp"
#include "windows/DataWindow.hpp"
#include "windows/DebugWindow.hpp"
#include "windows/MeasureWindow.hpp"
#include "windows/SettingsWindow.hpp"
#include "windows/StatisticsWindow.hpp"
#include "windows/ViewWindow.hpp"
#include "windows/plotting/HeatmapPlottingWindow.hpp"
#include "windows/plotting/PlottingWindow.hpp"
#include "windows/plotting/TimeseriesPlottingWindow.hpp"

namespace spq
{
    class SPARQ
    {
    public:
        using IDType = std::size_t;

        SPARQ();

        int init();
        int window_init();
        void register_windows();
        void update_windows();
        int run();
        int close_app();
        static void update_notifications() noexcept;

    private:
        constexpr void add_plotting_window(plotting::plot_type const type)
        {
            m_plotting_windows.emplace_back(create_plotting_window(type, m_next_id++));
        }

        std::unique_ptr<ui::PlottingWindow> create_plotting_window(plotting::plot_type const type, IDType const id)
        {
            switch (type)
            {
            default:
            case plotting::plot_type::timeseries:
            {
                return std::make_unique<ui::TimeseriesPlottingWindow>(m_data_handler, id);
            }
            case plotting::plot_type::heatmap:
            {
                return std::make_unique<ui::HeatmapPlottingWindow>(m_data_handler, id);
            }
            }
        }

        [[nodiscard]]
        constexpr std::optional<std::reference_wrapper<ui::PlottingWindow>> find_plot_by_id(IDType id) const noexcept;

        using PlotData = std::pair<std::unordered_set<std::size_t>&, plotting::plot_settings&>;

        [[nodiscard]]
        constexpr std::optional<PlotData> get_selected_plot_data() const noexcept
        {
            if (!m_selected_plot_id)
            {
                return std::nullopt;
            }

            if (auto const& plot = find_plot_by_id(*m_selected_plot_id))
            {
                return {
                    {plot->get().ids_to_plot(), plot->get().settings()}
                };
            }

            return std::nullopt;
        }

        constexpr void cleanup_closed_plotting_windows() noexcept;

        Serial m_sp;
        ui::ConsoleWindow m_console_window;
        data::DataHandler m_data_handler;
        ui::ConnectionWindow m_connection_window;
        ui::DataWindow m_data_window;
        ui::MeasureWindow m_measure_window;
        ui::ViewWindow m_view_window;
        ui::StatisticsWindow m_statistics_window;
        ui::SettingsWindow m_settings_window;
        ui::DebugWindow m_debug_window;

        sf::RenderWindow m_render_window;

        std::vector<std::reference_wrapper<ui::Window>> m_fixed_windows;
        std::vector<std::unique_ptr<ui::PlottingWindow>> m_plotting_windows;

        IDType m_next_id{};
        std::optional<IDType> m_selected_plot_id{std::nullopt};
    };
}
