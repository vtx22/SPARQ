#pragma once

#include "PlottingWindow.hpp"

namespace spq::ui
{
    class TimeseriesPlottingWindow final : public PlottingWindow
    {
        TimeseriesPlottingWindow(data::DataHandler& data_handler, std::size_t const id)
            : PlottingWindow(
                  std::string(ICON_FA_CHART_LINE "  Plot - Timeseries###PlottingWindow") + std::to_string(id),
                  data_handler,
                  id)
        {
        }
    };
}
