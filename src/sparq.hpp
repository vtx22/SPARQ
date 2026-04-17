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
    int run();
    int close_app();

private:
    Serial _sp;
    ConsoleWindow _console_window;
    DataHandler _data_handler;
    ConnectionWindow _connection_window;
    PlottingWindow _plotting_window;
    DataWindow _data_window;
    MeasureWindow _measure_window;
    ViewWindow _view_window;
    StatisticsWindow _statistics_window;
    SettingsWindow _settings_window;
    DebugWindow _debug_window;

    std::vector<std::reference_wrapper<Window>> _windows;

    sf::RenderWindow _render_window;
};
