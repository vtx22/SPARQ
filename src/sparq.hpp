#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <stdint.h>
#include <windows.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "imgui-SFML.h"
#include "imgui.h"
#include "implot.h"

#include "serial.hpp"

#include "sparq_config.h"
#include "sparq_types.hpp"

#include "DataHandler.hpp"

#include "ConsoleWindow.hpp"
#include "ConnectionWindow.hpp"
#include "PlottingWindow.hpp"

class SPARQ
{
public:
    SPARQ();

    int run();
    int close_app(sf::RenderWindow &window);

private:
};