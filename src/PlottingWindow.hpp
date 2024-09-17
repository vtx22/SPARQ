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
#include <cmath>

class PlottingWindow
{

public:
    PlottingWindow(DataHandler *data_handler);
    ~PlottingWindow();

    void update();
    void update_axes();
    void config_limits_n_values();

    std::tuple<std::vector<double> *, std::vector<double> *> get_xy_values(sparq_dataset_t &dataset);

private:
    DataHandler *_data_handler;
};
