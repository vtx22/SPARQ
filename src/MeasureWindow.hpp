#pragma once

#include <algorithm>

#include "imgui.h"
#include "implot.h"

#include "imgui-SFML.h"

#include "sparq_types.hpp"

#include "DataHandler.hpp"

#include "IconsFontAwesome6.h"

class MeasureWindow
{
public:
    MeasureWindow(DataHandler *data_handler);
    ~MeasureWindow();

    void update();
    void measure_markers_table(std::vector<sparq_marker_t> &markers);

private:
    DataHandler *_data_handler;
};
