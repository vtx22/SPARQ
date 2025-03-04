#pragma once

#include "Window.hpp"

class StatisticsWindow : public Window
{
public:
    StatisticsWindow(DataHandler *data_handler) : Window(ICON_FA_MAGNIFYING_GLASS_CHART "  Statistics", data_handler) {};

    void update_content();
};