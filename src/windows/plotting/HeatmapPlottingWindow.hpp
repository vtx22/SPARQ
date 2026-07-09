#pragma once

#include "PlottingWindow.hpp"

namespace spq::ui
{
    class HeatmapPlottingWindow final : public PlottingWindow
    {
    public:
        HeatmapPlottingWindow(data::DataHandler& data_handler, std::size_t const id)
            : PlottingWindow(
                  std::string(ICON_FA_CHART_LINE "  Plot - Heatmap###PlottingWindow") + std::to_string(id),
                  data_handler,
                  id)
        {
        }

        plotting::plot_settings& settings() override
        {
            return m_settings;
        }

    private:
        plotting::timeseries_settings m_settings{};

    protected:
        void update_plot_contents(data::Datasets& datasets) override
        {
        }

        [[nodiscard]]
        constexpr ImPlotFlags get_plot_flags() const override
        {
            return ImPlotFlags_NoMenus;
        }
    };
}
