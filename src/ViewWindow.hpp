#pragma once

#include "imgui.h"
#include "implot.h"
#include "imgui-SFML.h"
#include "sparq_types.hpp"

#include "IconsFontAwesome6.h"

#include "DataHandler.hpp"

class ViewWindow
{
public:
    ViewWindow(DataHandler *data_handler);
    ~ViewWindow();
    void update();

    void menu_plot_type();
    void menu_x_axis();
    void menu_y_axis();

private:
    DataHandler *_data_handler;
};