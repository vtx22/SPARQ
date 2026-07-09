#pragma once

#include "PlottingWindow.hpp"

namespace spq::ui
{
    class TimeseriesPlottingWindow final : public PlottingWindow
    {
    public:
        TimeseriesPlottingWindow(data::DataHandler& data_handler, std::size_t const id)
            : PlottingWindow(
                  std::string(ICON_FA_CHART_LINE "  Plot - Timeseries###PlottingWindow") + std::to_string(id),
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
            uint32_t max_samples = std::stoi(m_config_handler.ini["downsampling"]["max_samples"]);

            if (m_config_handler.ini["downsampling"]["max_samples_type"] == "0" && !datasets.empty())
            {
                max_samples = static_cast<uint32_t>(std::round(max_samples / static_cast<double>(datasets.size())));
            }

            std::size_t i = 0;
            for (auto& ds : datasets.data())
            {
                if (!m_ids_to_plot.contains(ds.id))
                {
                    continue;
                }

                std::string name = (ds.name[0] == 0) ? std::to_string(ds.id) : std::string(ds.name);
                ImPlot::SetNextLineStyle(ds.color, 3);

                // auto [x_values, y_values] = get_xy_downsampled(ds, max_samples, ImPlot::GetPlotLimits().X.Min, ImPlot::GetPlotLimits().X.Max);
                //
                // if (ds.display_square)
                // {
                //     ImPlot::PlotStairs(
                //         (name + "###LP" + std::to_string(ds.id)).c_str(),
                //         x_values.data(),
                //         y_values.data(),
                //         y_values.size());
                // }
                // else
                // {
                //     ImPlot::PlotLine(
                //         (name + "###LP" + std::to_string(ds.id)).c_str(),
                //         x_values.data(),
                //         y_values.data(),
                //         y_values.size());
                // }

                i++;
            }
        }

        [[nodiscard]]
        constexpr ImPlotFlags get_plot_flags() const override
        {
            return ImPlotFlags_NoMenus;
        }
    };
}
