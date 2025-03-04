#pragma once

#include "Window.hpp"

#include <algorithm>

#include "implot.h"
#include "imgui-SFML.h"
#include "sparq_types.hpp"

class MeasureWindow : public Window
{
public:
    MeasureWindow(DataHandler *data_handler) : Window(ICON_FA_RULER_HORIZONTAL "  Measure", data_handler) {}

    void update_content();
    void measure_markers_table(std::vector<sparq_marker_t> &markers);
};
