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
        constexpr void update_axes() const
        {
            using namespace spq::plotting;

            auto const x_label = x_unit_labels[static_cast<uint8_t>(m_settings.x_unit)];
            switch (m_settings.x_fit)
            {
            case x_fit_t::manual:
            {
                ImPlot::SetupAxis(ImAxis_X1, x_label, 0);
                break;
            }
            case x_fit_t::all:
            {
                ImPlot::SetupAxis(ImAxis_X1, x_label, ImPlotAxisFlags_AutoFit);
                break;
            }
            default:
                break;
            }

            switch (m_settings.x_unit)
            {
            case x_unit_t::absolute_time:
            {
                ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
                break;
            }
            default:
                break;
            }

            switch (m_settings.y_fit)
            {
            case y_fit_t::manual:
            {
                ImPlot::SetupAxis(ImAxis_Y1, nullptr, 0);
                break;
            }
            case y_fit_t::all:
            {
                ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);
                break;
            }
            default:
                break;
            }
        }

        plotting::timeseries_settings m_settings{};

    protected:
        void update_plot_contents(data::Datasets& datasets) override
        {
            update_axes();

            uint32_t max_samples = std::stoi(m_config_handler.ini["downsampling"]["max_samples"]);

            if (m_config_handler.ini["downsampling"]["max_samples_type"] == "0" && !datasets.empty())
            {
                max_samples = static_cast<uint32_t>(std::round(max_samples / static_cast<double>(datasets.size())));
            }

            for (auto& ds : datasets.data())
            {
                if (!m_ids_to_plot.contains(ds.id))
                {
                    continue;
                }

                std::string name = (ds.name[0] == 0) ? std::to_string(ds.id) : std::string(ds.name);
                ImPlot::SetNextLineStyle(ds.color, 3);

                auto [x_values, y_values] = get_xy_downsampled(ds, max_samples, ImPlot::GetPlotLimits().X.Min, ImPlot::GetPlotLimits().X.Max);

                if (ds.display_square)
                {
                    ImPlot::PlotStairs(
                        (name + "###LP" + std::to_string(ds.id)).c_str(),
                        x_values.data(),
                        y_values.data(),
                        y_values.size());
                }
                else
                {
                    ImPlot::PlotLine(
                        (name + "###LP" + std::to_string(ds.id)).c_str(),
                        x_values.data(),
                        y_values.data(),
                        y_values.size());
                }
            }
        }

        std::pair<std::vector<double>&, std::vector<double>&> get_xy_downsampled(
            data::dataset_t& dataset,
            std::size_t const max_samples,
            double x_min,
            double x_max) const
        {
            static std::vector<double> x_downsampled, y_downsampled;
            static std::vector<double> x_in_view, y_in_view;

            auto& d = dataset;

            auto const downsampling_enabled = (m_config_handler.ini["downsampling"]["enabled"] == "1");
            // No downsampling possible
            if (d.samples.empty() || d.samples.size() < max_samples || !downsampling_enabled)
            {
                return {d.samples, d.y_values};
            }

            // All data out of view
            if (d.samples.back() < x_min || d.samples.front() > x_max)
            {
                x_downsampled.resize(0);
                y_downsampled.resize(0);
                return {x_downsampled, y_downsampled};
            }

            // For FIT_ALL x axis we have to keep the whole data length so ImPlot AutoFit works
            if (m_data_handler.x_fit_select == 1)
            {
                x_min = std::numeric_limits<double>::lowest();
                x_max = std::numeric_limits<double>::max();
            }

            std::vector<double>* x_values;
            switch (m_data_handler.x_axis_select)
            {
            default:
            case 0:
                x_values = &d.samples;
                break;
            case 1:
                x_values = &d.relative_times;
                break;
            case 2:
                x_values = &d.absolute_times;
                break;
            }

            // Find the sample index for the x_value that is below x_min / above x_max
            auto const lower = std::ranges::lower_bound(x_values->begin(), x_values->end(), x_min);
            auto const upper = std::ranges::upper_bound(x_values->begin(), x_values->end(), x_max);

            std::size_t min_index = std::distance(x_values->begin(), lower);
            std::size_t max_index = std::distance(x_values->begin(), upper) + 1;

            if (min_index > 0)
            {
                min_index--;
            }
            if (max_index >= x_values->size())
            {
                max_index = x_values->size() - 1;
            }

            std::size_t number_of_samples = max_index - min_index;

            // Keep only the values that are visible
            x_in_view.resize(number_of_samples);
            y_in_view.resize(number_of_samples);
            std::copy(x_values->begin() + min_index, x_values->begin() + max_index, x_in_view.begin());
            std::copy(d.y_values.begin() + min_index, d.y_values.begin() + max_index, y_in_view.begin());

            // There are less than max_samples values in view, so no downsampling is needed
            if (max_samples >= number_of_samples)
            {
                return {x_in_view, y_in_view};
            }

            x_downsampled.resize(max_samples);
            y_downsampled.resize(max_samples);

            x_downsampled[0] = x_in_view.front();
            y_downsampled[0] = y_in_view.front();

            auto const step = static_cast<double>(number_of_samples - 1) / (max_samples - 1);

            // Start from the second value
            auto index = step;

            for (std::size_t i = 1; i < max_samples - 1; ++i)
            {
                x_downsampled[i] = (x_in_view[static_cast<size_t>(index)]);
                y_downsampled[i] = (y_in_view[static_cast<size_t>(index)]);
                index += step;
            }

            // Always keep the last value
            x_downsampled[max_samples - 1] = x_in_view.back();
            y_downsampled[max_samples - 1] = y_in_view.back();

            return {x_downsampled, y_downsampled};
        }

        [[nodiscard]]
        constexpr ImPlotFlags get_plot_flags() const override
        {
            return ImPlotFlags_NoMenus;
        }
    };
}
