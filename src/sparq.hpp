#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <stdint.h>
#include <windows.h>
#include <dwmapi.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "imgui-SFML.h"
#include "imgui.h"
#include "implot.h"

#include "serial.hpp"
#include "imstyles.hpp"

#include "sparq_config.h"
#include "sparq_types.hpp"

#include "DataHandler.hpp"

#include "ConsoleWindow.hpp"
#include "ConnectionWindow.hpp"
#include "PlottingWindow.hpp"
#include "DataWindow.hpp"

class SPARQ
{
public:
    SPARQ();

    int init();
    void object_init();
    int window_init();
    int run();
    int close_app();

private:
    Serial *_sp;
    DataHandler *_data_handler;
    ConsoleWindow *_console_window;
    ConnectionWindow *_connection_window;
    PlottingWindow *_plotting_window;
    DataWindow *_data_window;

    sf::RenderWindow *_window;
};