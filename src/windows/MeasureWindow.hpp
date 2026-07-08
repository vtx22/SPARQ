#pragma once

#include "../sparq_types.hpp"
#include "Window.hpp"
#include "imgui-SFML.h"
#include "implot.h"

namespace spq::ui
{
    class MeasureWindow final : public Window
    {
    public:
        explicit MeasureWindow(data::DataHandler& data_handler)
            : Window(ICON_FA_RULER_HORIZONTAL "  Measure", data_handler)
        {
        }

    private:
        void measure_markers_table(std::vector<sparq_marker_t>& markers, data::Datasets& datasets) const;

    protected:
        void update_content(data::Datasets& datasets) override;
    };
}
