#pragma once

#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"

#include "sparq_types.hpp"
#include "DataHandler.hpp"

#include <vector>
#include <string>
#include <cstdint>
#include <tuple>

class PlottingWindow
{

public:
    PlottingWindow(DataHandler *data_handler);
    ~PlottingWindow();

    void update();

    std::tuple<std::vector<double> *, std::vector<double> *> get_xy_values(sparq_dataset_t &dataset);

private:
    DataHandler *_data_handler;
};
