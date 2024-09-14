#pragma once

#include "imgui.h"
#include "implot.h"
#include "implot_internal.h"

#include "sparq_types.hpp"

#include <vector>
#include <string>
#include <cstdint>

enum SPARQ_X_TYPE
{
    SAMPLES,
    RELATIVE_TIME,
    ABSOLUTE_TIME,
};

class PlottingWindow
{

public:
    PlottingWindow();
    ~PlottingWindow();

    void update(std::vector<sparq_dataset_t> &datasets);

    void set_x_type(SPARQ_X_TYPE x_type);

private:
    SPARQ_X_TYPE _x_type = SAMPLES;
};
