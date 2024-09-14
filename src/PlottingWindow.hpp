#pragma once

#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"

#include "sparq_types.hpp"
#include "DataHandler.hpp"

#include <vector>
#include <string>
#include <cstdint>

class PlottingWindow
{

public:
    PlottingWindow(DataHandler * data_handler);
    ~PlottingWindow();

    void update();

private:
    DataHandler *_data_handler;
};
