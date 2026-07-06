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

#include "ConnectionWindow.hpp"
#include "ConsoleWindow.hpp"
#include "DataWindow.hpp"
#include "DebugWindow.hpp"
#include "MeasureWindow.hpp"
#include "PlottingWindow.hpp"
#include "SettingsWindow.hpp"
#include "StatisticsWindow.hpp"
#include "ViewWindow.hpp"

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
    constexpr void add_plotting_window();

    [[nodiscard]]
    constexpr std::optional<std::reference_wrapper<PlottingWindow>> find_plot_by_id(IDType id) const noexcept;
    [[nodiscard]]
    constexpr std::optional<std::reference_wrapper<spq::plotting::plot_settings_t>> get_selected_plot_settings() const noexcept;
    constexpr void cleanup_closed_plotting_windows() noexcept;

    Serial m_sp;
    ConsoleWindow m_console_window;
    DataHandler m_data_handler;
    ConnectionWindow m_connection_window;
    DataWindow m_data_window;
    MeasureWindow m_measure_window;
    ViewWindow m_view_window;
    StatisticsWindow m_statistics_window;
    SettingsWindow m_settings_window;
    DebugWindow m_debug_window;

    sf::RenderWindow m_render_window;

    std::vector<std::reference_wrapper<Window>> m_fixed_windows;
    std::vector<std::unique_ptr<PlottingWindow>> m_plotting_windows;

    IDType m_next_id{};
    std::optional<IDType> m_selected_plot_id{std::nullopt};
};
