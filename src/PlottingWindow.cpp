#include "PlottingWindow.hpp"

void PlottingWindow::update_content()
{
    std::lock_guard<std::mutex> lock(_data_handler->get_data_mutex());
    std::vector<sparq_dataset_t> &datasets = _data_handler->get_datasets_editable();

    ImPlotFlags plot_flags = ImPlotFlags_NoMenus;

    if (_data_handler->plot_settings.type == sparq_plot_t::HEATMAP && _data_handler->plot_settings.heatmap_settings.equal)
    {
        plot_flags |= ImPlotFlags_Equal;
    }

    if (ImPlot::BeginPlot("##Data", ImVec2(-1, -1), plot_flags))
    {
        update_axes();

        auto &markers = _data_handler->get_markers();
        for (uint32_t m = 0; m < markers.size(); m++)
        {
            if (markers[m].hidden)
            {
                continue;
            }

            ImPlot::DragLineX(m, &markers[m].x, markers[m].color, 2);
            ImPlot::TagX(markers[m].x, markers[m].color, markers[m].name.c_str());
        }

        ImPlotContext *ctx = ImPlot::GetCurrentContext();
        ImPlotPlot *plot = ctx->CurrentPlot;

        switch (_data_handler->plot_settings.type)
        {
        case sparq_plot_t::LINE:
        {
            uint8_t i = 0;

            uint32_t max_samples = std::stoi(_config_handler.ini["downsampling"]["max_samples"]);

            if (_config_handler.ini["downsampling"]["max_samples_type"] == "0" && datasets.size() != 0)
            {
                max_samples = static_cast<uint32_t>(round(max_samples / (double)datasets.size()));
            }

            for (auto &ds : datasets)
            {
                std::string name = (ds.name[0] == 0) ? std::to_string(ds.id) : std::string(ds.name);
                ImPlot::SetNextLineStyle(ds.color, 3);

                auto [x_values, y_values] = get_xy_downsampled(ds, max_samples, ImPlot::GetPlotLimits().X.Min, ImPlot::GetPlotLimits().X.Max);

                if (ds.display_square)
                {
                    ImPlot::PlotStairs((name + "###LP" + std::to_string(ds.id)).c_str(), x_values.data(), y_values.data(), y_values.size());
                }
                else
                {
                    ImPlot::PlotLine((name + "###LP" + std::to_string(ds.id)).c_str(), x_values.data(), y_values.data(), y_values.size());
                }

                ImPlotItem *item = plot->Items.GetLegendItem(i);
                if (ds.toggle_visibility)
                {
                    item->Show = !item->Show;
                    ds.toggle_visibility = false;
                }

                ds.hidden = !item->Show;

                i++;
            }
            break;
        }
        case sparq_plot_t::HEATMAP:
        {
            sparq_heatmap_settings_t &hms = _data_handler->plot_settings.heatmap_settings;

            std::vector<float> values(hms.cols * hms.rows);

            for (uint32_t i = 0; i < hms.cols * hms.rows; i++)
            {
                if (i >= datasets.size())
                {
                    break;
                }
                values[i] = datasets[i].y_values.back();
            }

            uint32_t bounds_max_x = hms.normalize_xy ? 1 : hms.cols;
            uint32_t bounds_max_y = hms.normalize_xy ? 1 : hms.rows;

            ImPlot::PlotHeatmap("Heatmap", values.data(), hms.rows, hms.cols, hms.scale_min, hms.scale_max, hms.show_values ? "%.1f" : "", {0, 0}, {bounds_max_x, bounds_max_y});
            break;
        }
        }
    }

    ImPlot::EndPlot();
}

std::pair<std::vector<double> &, std::vector<double> &> PlottingWindow::get_xy_downsampled(sparq_dataset_t &dataset, uint32_t max_samples, double x_min, double x_max)
{
    auto &d = dataset;

    bool downsampling_enabled;
    std::istringstream(_config_handler.ini["downsampling"]["enabled"]) >> std::boolalpha >> downsampling_enabled;

    // No downsampling possible
    if (d.samples.empty() || d.samples.size() < max_samples || !downsampling_enabled)
    {
        return {d.samples, d.y_values};
    }

    // All data out of view
    if (d.samples.back() < x_min || d.samples.front() > x_max)
    {
        _x_downsampled.resize(0);
        _y_downsampled.resize(0);
        return {_x_downsampled, _y_downsampled};
    }

    // For FIT_ALL x axis we have to keep the whole data length so ImPlot AutoFit works
    if (_data_handler->x_fit_select == 1)
    {
        x_min = -DBL_MAX;
        x_max = DBL_MAX;
    }

    std::vector<double> *x_values;
    switch (_data_handler->x_axis_select)
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
    auto lower = std::lower_bound(x_values->begin(), x_values->end(), x_min);
    auto upper = std::upper_bound(x_values->begin(), x_values->end(), x_max);

    size_t min_index = std::distance(x_values->begin(), lower);
    size_t max_index = std::distance(x_values->begin(), upper) + 1;

    if (min_index > 0)
    {
        min_index--;
    }
    if (max_index >= x_values->size())
    {
        max_index = x_values->size() - 1;
    }

    size_t number_of_samples = max_index - min_index;

    // Keep only the values that are visible
    _x_in_view.resize(number_of_samples);
    _y_in_view.resize(number_of_samples);
    std::copy(x_values->begin() + min_index, x_values->begin() + max_index, _x_in_view.begin());
    std::copy(d.y_values.begin() + min_index, d.y_values.begin() + max_index, _y_in_view.begin());

    // There are less than max_samples values in view, so no downsampling is needed
    if (max_samples >= number_of_samples)
    {
        return {_x_in_view, _y_in_view};
    }

    _x_downsampled.resize(max_samples);
    _y_downsampled.resize(max_samples);

    _x_downsampled[0] = _x_in_view.front();
    _y_downsampled[0] = _y_in_view.front();

    double step = static_cast<double>(number_of_samples - 1) / (max_samples - 1);

    // Start from the second value
    double index = step;

    for (size_t i = 1; i < max_samples - 1; ++i)
    {
        _x_downsampled[i] = (_x_in_view[static_cast<size_t>(index)]);
        _y_downsampled[i] = (_y_in_view[static_cast<size_t>(index)]);
        index += step;
    }

    // Always keep the last value
    _x_downsampled[max_samples - 1] = _x_in_view.back();
    _y_downsampled[max_samples - 1] = _y_in_view.back();

    return {_x_downsampled, _y_downsampled};
}

void PlottingWindow::update_axes()
{

    // X Axis Setup
    const char *x_axis_label = x_axis_types[_data_handler->x_axis_select].axis_label;

    if (_data_handler->x_axis_select == 2)
    {
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
    }

    switch (_data_handler->x_fit_select)
    {
    // Manual
    case 0:
        ImPlot::SetupAxis(ImAxis_X1, x_axis_label, 0);
        break;
    // Fit all
    default:
    case 1:
        ImPlot::SetupAxis(ImAxis_X1, x_axis_label, ImPlotAxisFlags_AutoFit);
        break;
    // Last N values
    case 2:
        config_limits_n_values();
        break;
    }

    // Y Axis Setup
    switch (_data_handler->y_fit_select)
    {
    // Manual
    case 0:
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, 0);
        break;
    // Fit all
    case 1:
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);
        break;
    }
}

void PlottingWindow::config_limits_n_values()
{

    switch (_data_handler->x_axis_select)
    {
    // Last N Samples
    case 0:
    {
        int max_sample = round(_data_handler->get_max_sample()) - 1;
        int min_sample = max_sample - _data_handler->last_n + 1;

        if (min_sample < 0)
        {
            min_sample = 0;
        }

        ImPlot::SetupAxisLimits(ImAxis_X1, min_sample, max_sample, ImPlotCond_Always);
        break;
    }

    // Last N relative seconds
    case 1:
    {
        double max_rel_time = _data_handler->get_max_rel_time();
        double min_rel_time = max_rel_time - _data_handler->last_n;

        if (min_rel_time < 0)
        {
            min_rel_time = 0;
        }

        ImPlot::SetupAxisLimits(ImAxis_X1, min_rel_time, max_rel_time, ImPlotCond_Always);
        break;
    }

        // Last N absolute seconds
    case 2:
    {
        double max_abs_time = _data_handler->get_max_abs_time();
        double min_abs_time = max_abs_time - _data_handler->last_n;

        if (min_abs_time < 0)
        {
            min_abs_time = 0;
        }

        ImPlot::SetupAxisLimits(ImAxis_X1, min_abs_time, max_abs_time, ImPlotCond_Always);
        break;
    }
    }
}