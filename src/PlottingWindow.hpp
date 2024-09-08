#ifndef PLOTTING_WINDOW_HPP
#define PLOTTING_WINDOW_HPP

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

    void add_dataset(const sparq_dataset &dataset);

    void update();

private:
    std::vector<sparq_dataset> _datasets;
};

#endif // PLOTTING_WINDOW_HPP