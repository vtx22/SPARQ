#pragma once

#include "Window.hpp"

#include "imgui-SFML.h"
#include "implot.h"
#include "sparq_types.hpp"

class ViewWindow : public Window
{
public:
    ViewWindow(DataHandler* data_handler)
        : Window(ICON_FA_SLIDERS "  View", data_handler)
    {
    }

    void update_content();

    void menu_plot_type();
    void menu_x_axis();
    void menu_y_axis();
};
