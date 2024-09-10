#pragma once

#include "imgui.h"
#include "implot.h"

#include "sparq_types.hpp"

#include <vector>
#include <string>
#include <cstdint>

class PlottingWindow
{

public:
    PlottingWindow();
    ~PlottingWindow();

    void update(const std::vector<sparq_dataset_t> &datasets);

private:
};
