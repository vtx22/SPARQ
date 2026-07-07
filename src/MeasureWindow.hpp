#pragma once

#include "Window.hpp"
#include "imgui-SFML.h"
#include "implot.h"
#include "sparq_types.hpp"

class MeasureWindow final : public Window
{
public:
    explicit MeasureWindow(DataHandler& data_handler)
        : Window(ICON_FA_RULER_HORIZONTAL "  Measure", data_handler)
    {
    }

private:
    void measure_markers_table(std::vector<sparq_marker_t>& markers, Datasets& datasets) const;

protected:
    void update_content(Datasets& datasets) override;
};
