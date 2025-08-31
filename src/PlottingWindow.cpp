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

        size_t id = 0;
        for (auto &m : markers)
        {
            if (m.hidden)
            {
                continue;
            }

            ImPlot::DragLineX(id++, &m.x, m.color, 2);
            ImPlot::TagX(m.x, m.color, m.name.c_str());
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

            float min_scale = hms.scale_min;
            float max_scale = hms.scale_max;

            if (hms.autoscale)
            {
                auto [min_it, max_it] = std::minmax_element(values.begin(), values.end());
                min_scale = *min_it;
                max_scale = *max_it;
            }

            if (hms.invert_scale)
            {
                float tmp = min_scale;
                min_scale = max_scale;
                max_scale = tmp;
            }

            int rows = hms.rows;
            int cols = hms.cols;

            if (hms.smooth)
            {
                values = bilinear_interpolate(values, hms.rows, hms.cols, hms.smoothing_factor);
                rows = hms.rows * hms.smoothing_factor;
                cols = hms.cols * hms.smoothing_factor;
                bounds_max_x = cols;
                bounds_max_y = rows;
            }

            ImPlot::PlotHeatmap("Heatmap", values.data(), rows, cols, min_scale, max_scale, hms.show_values ? "%.1f" : "", {0, 0}, {bounds_max_x, bounds_max_y}, 0);
            break;
        }
        }
    }

    ImPlot::EndPlot();
}

std::pair<std::vector<double> &, std::vector<double> &> PlottingWindow::get_xy_downsampled(sparq_dataset_t &dataset, uint32_t max_samples, double x_min, double x_max)
{
    auto &d = dataset;

    bool downsampling_enabled = _config_handler.ini["downsampling"]["enabled"] == "1";
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

std::vector<float> PlottingWindow::bilinear_interpolate(const std::vector<float> &original_image, int original_rows, int original_cols, float scale_factor)
{

    int new_rows = static_cast<int>(original_rows * scale_factor);
    int new_cols = static_cast<int>(original_cols * scale_factor);

    std::vector<float> interpolated_image(new_rows * new_cols);

    // Helper lambda to get pixel value with bounds checking
    auto get_pixel = [&](int row, int col) -> float
    {
        if (row < 0 || row >= original_rows || col < 0 || col >= original_cols)
        {
            return 0.0f;
        }
        return original_image[row * original_cols + col];
    };

    // Perform bilinear interpolation
    for (int new_row = 0; new_row < new_rows; ++new_row)
    {
        for (int new_col = 0; new_col < new_cols; ++new_col)
        {

            // Map new coordinates back to original image coordinates
            float orig_row_f = (new_row + 0.5f) / scale_factor - 0.5f;
            float orig_col_f = (new_col + 0.5f) / scale_factor - 0.5f;

            // Get the four surrounding pixel coordinates
            int row0 = static_cast<int>(std::floor(orig_row_f));
            int row1 = row0 + 1;
            int col0 = static_cast<int>(std::floor(orig_col_f));
            int col1 = col0 + 1;

            // Calculate interpolation weights
            float row_weight = orig_row_f - row0;
            float col_weight = orig_col_f - col0;

            // Get the four corner pixel values
            float top_left = get_pixel(row0, col0);
            float top_right = get_pixel(row0, col1);
            float bottom_left = get_pixel(row1, col0);
            float bottom_right = get_pixel(row1, col1);

            // Perform bilinear interpolation
            float top_interp = top_left * (1.0f - col_weight) + top_right * col_weight;
            float bottom_interp = bottom_left * (1.0f - col_weight) + bottom_right * col_weight;
            float final_value = top_interp * (1.0f - row_weight) + bottom_interp * row_weight;

            interpolated_image[new_row * new_cols + new_col] = final_value;
        }
    }

    return interpolated_image;
}