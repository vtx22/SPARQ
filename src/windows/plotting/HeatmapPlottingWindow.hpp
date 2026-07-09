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
        plotting::heatmap_settings m_settings{};

    protected:
        void update_plot_contents(data::Datasets& datasets) override
        {
            auto const pixel_count = static_cast<std::size_t>(m_settings.rows * m_settings.cols);
            std::vector<float> values(pixel_count);
            for (std::size_t i{}; auto const& ds : datasets.data())
            {
                if (i >= pixel_count)
                {
                    break;
                }
                values[i++] = ds.y_values.back();
            }

            auto scale_min{m_settings.scale_min};
            auto scale_max{m_settings.scale_max};

            if (m_settings.autoscale)
            {
                auto [min, max] = std::ranges::minmax(values);
                scale_min = min;
                scale_max = max;
            }

            ImPlot::PlotHeatmap(
                "Heatmap",
                values.data(),
                m_settings.rows,
                m_settings.cols,
                scale_min,
                scale_max,
                m_settings.show_values ? "%.1f" : "",
                {0, 0},
                {
                    m_settings.normalize_xy ? 1.0 : m_settings.cols,
                    m_settings.normalize_xy ? 1.0 : m_settings.rows,
                },
                0);
        }

        [[nodiscard]]
        constexpr ImPlotFlags get_plot_flags() const override

        {
            return ImPlotFlags_NoMenus;
        }
    };
}
