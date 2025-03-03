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
#include <utility>
#include <algorithm>

#include "IconsFontAwesome6.h"

class PlottingWindow
{

public:
    PlottingWindow(DataHandler *data_handler);
    ~PlottingWindow();

    void update();
    void update_axes();
    void config_limits_n_values();

    std::pair<std::vector<double> &, std::vector<double> &> get_xy_downsampled(sparq_dataset_t &dataset, uint32_t max_samples, double x_min, double x_max);

private:
    DataHandler *_data_handler;

    std::vector<double> _x_downsampled, _x_in_view;
    std::vector<double> _y_downsampled, _y_in_view;
};
