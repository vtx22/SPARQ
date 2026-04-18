#pragma once

#include "Window.hpp"
#include "imgui-SFML.h"
#include "implot.h"
#include "sparq_types.hpp"

class ViewWindow : public Window
{
public:
    using CreatePlottingWindowCallback = std::function<void()>;

    ViewWindow(
        DataHandler& data_handler,
        CreatePlottingWindowCallback on_create_plotting_window)
        : Window(ICON_FA_SLIDERS "  View", data_handler),
          _on_create_plotting_window(std::move(on_create_plotting_window))
    {
    }

    void update_content();

    void menu_plot_type();
    void menu_x_axis();
    void menu_y_axis();

private:
    CreatePlottingWindowCallback _on_create_plotting_window;
};
