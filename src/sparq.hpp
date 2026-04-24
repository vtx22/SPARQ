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
    SPARQ();

    int init();
    int window_init();
    void register_windows();
    void update_windows();
    int run();
    int close_app();

private:
    constexpr void add_plotting_window();
    constexpr std::optional<std::reference_wrapper<PlottingWindow>> find_plot_by_id(std::size_t id) noexcept;
    constexpr std::optional<std::reference_wrapper<spq::plotting::plot_settings>> get_selected_plot_settings() noexcept;
    constexpr void select_plot(std::size_t id) noexcept;
    constexpr void highlight_selected_plot_only() noexcept;
    constexpr void cleanup_closed_plotting_windows() noexcept;

    Serial _sp;
    ConsoleWindow _console_window;
    DataHandler _data_handler;
    ConnectionWindow _connection_window;
    DataWindow _data_window;
    MeasureWindow _measure_window;
    ViewWindow _view_window;
    StatisticsWindow _statistics_window;
    SettingsWindow _settings_window;
    DebugWindow _debug_window;

    sf::RenderWindow _render_window;

    std::vector<std::reference_wrapper<Window>> _windows;
    std::vector<std::unique_ptr<PlottingWindow>> _plotting_windows;

    std::size_t _next_id{};
    std::optional<std::size_t> _selected_plot_id{std::nullopt};
};
