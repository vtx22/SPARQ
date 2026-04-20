#include "PlottingWindow.hpp"

void PlottingWindow::update_content()
{
    std::lock_guard<std::mutex> lock(_data_handler.get_data_mutex());

    _in_focus_flag = ImGui::IsWindowFocused();

    update_window_name(); // TODO: Update only on plot type change
    update_plot_contents();
    show_highlighting_rectangle();
}

void PlottingWindow::update_window_name()
{
    using namespace spq::plotting;

    _name = internal::window_name_prefix;
    _name += plot_type_names.at(static_cast<uint8_t>(_plot_settings.type));
    _name += internal::window_name_id_prefix;
    _name += std::to_string(_id);
}

void PlottingWindow::update_plot_contents()
{
    using namespace spq::plotting;

    if (ImPlot::BeginPlot("##Plot", ImVec2(-1, -1), get_plot_flags()))
    {
        update_markers();

        switch (_plot_settings.type)
        {
        case plot_type::timeseries:
            handle_plot_timeseries();
            break;
        case plot_type::xy:
            handle_plot_xy();
            break;
        case plot_type::single_value:
            handle_plot_single_value();
            break;
        case plot_type::heatmap:
            handle_plot_heatmap();
            break;
        default:
            break;
        }

        ImPlot::EndPlot();
    }
}

void PlottingWindow::handle_plot_timeseries()
{
    auto& datasets = _data_handler.get_datasets_editable();

    ImPlotContext* ctx = ImPlot::GetCurrentContext();
    ImPlotPlot* plot = ctx->CurrentPlot;

    uint32_t max_samples = std::stoi(_config_handler.ini["downsampling"]["max_samples"]);

    if (_config_handler.ini["downsampling"]["max_samples_type"] == "0" && datasets.size() != 0)
    {
        max_samples = static_cast<uint32_t>(std::round(max_samples / static_cast<double>(datasets.size())));
    }

    std::size_t i = 0;
    for (auto& ds : datasets)
    {
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

        ImPlotItem* item = plot->Items.GetLegendItem(i);
        if (ds.toggle_visibility)
        {
            item->Show = !item->Show;
            ds.toggle_visibility = false;
        }
        if (ds.hide)
        {
            item->Show = false;
            ds.hide = false;
        }
        if (ds.show)
        {
            item->Show = true;
            ds.show = false;
        }

        ds.hidden = !item->Show;

        i++;
    }
}

void PlottingWindow::handle_plot_xy()
{
}

void PlottingWindow::handle_plot_single_value()
{
}

void PlottingWindow::handle_plot_heatmap()
{
    auto& datasets = _data_handler.get_datasets_editable();

    auto const& hms = _data_handler.plot_settings.heatmap_settings;

    std::vector<float> values(hms.cols * hms.rows);

    for (std::size_t i = 0; i < hms.cols * hms.rows; i++)
    {
        if (i >= datasets.size())
        {
            break;
        }

        values[i] = datasets[i].y_values.back();
    }

    uint32_t bounds_max_x = hms.normalize_xy ? 1 : hms.cols;
    uint32_t bounds_max_y = hms.normalize_xy ? 1 : hms.rows;

    auto min_scale = hms.scale_min;
    auto max_scale = hms.scale_max;

    if (hms.autoscale)
    {
        auto const [min_it, max_it] = std::minmax_element(values.begin(), values.end());
        min_scale = *min_it;
        max_scale = *max_it;
    }

    if (hms.invert_scale)
    {
        auto const tmp = min_scale;
        min_scale = max_scale;
        max_scale = tmp;
    }

    auto rows = hms.rows;
    auto cols = hms.cols;

    if (hms.smooth)
    {
        values = bilinear_interpolate(values, hms.rows, hms.cols, hms.smoothing_factor);
        rows = hms.rows * hms.smoothing_factor;
        cols = hms.cols * hms.smoothing_factor;
        bounds_max_x = cols;
        bounds_max_y = rows;
    }

    ImPlot::PlotHeatmap(
        "Heatmap",
        values.data(),
        rows,
        cols,
        min_scale,
        max_scale,
        hms.show_values ? "%.1f" : "",
        {0, 0},
        {static_cast<double>(bounds_max_x), static_cast<double>(bounds_max_y)},
        0);
}

void PlottingWindow::update_markers()
{
    auto& markers = _data_handler.get_markers();

    std::size_t id = 0;
    for (auto& m : markers)
    {
        if (m.hidden)
        {
            continue;
        }

        ImPlot::DragLineX(id++, &m.x, m.color, 2);
        ImPlot::TagX(m.x, m.color, m.name.c_str());
    }
}

constexpr void PlottingWindow::show_highlighting_rectangle() const
{
    if (!_highlight_window)
    {
        return;
    }

    auto constexpr border_width = spq::styling::plot_highlight_border_width;
    auto p0 = ImGui::GetWindowPos();
    auto const sz = ImGui::GetWindowSize();
    ImVec2 p1{p0.x + sz.x, p0.y + sz.y};

    if (ImGui::IsWindowDocked())
    {
        p0.x += border_width;
        p0.y += ImGui::GetFrameHeight();
        p1.x += border_width;
        p1.y -= border_width;
    }

    ImGui::GetForegroundDrawList()->AddRect(
        p0,
        p1,
        spq::styling::plot_highlight_color,
        ImGui::GetStyle().WindowRounding,
        0,
        border_width);
}

ImPlotFlags PlottingWindow::get_plot_flags()
{
    ImPlotFlags plot_flags = ImPlotFlags_NoMenus;

    if (_data_handler.plot_settings.type == spq::plotting::plot_type::heatmap && _plot_settings.equal)
    {
        plot_flags |= ImPlotFlags_Equal;
    }

    return plot_flags;
}

void PlottingWindow::before_imgui_begin()
{
    // Highlight selected plot
    _highlight_window = _in_focus_flag;

    if (_highlight_window)
    {
        constexpr auto color = spq::styling::plot_highlight_color;
        ImGui::PushStyleColor(ImGuiCol_TitleBg, color);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, color);
        ImGui::PushStyleColor(ImGuiCol_Tab, color);
        ImGui::PushStyleColor(ImGuiCol_TabActive, color);
        ImGui::PushStyleColor(ImGuiCol_TabHovered, color);
    }
}

void PlottingWindow::after_imgui_end()
{
    if (_highlight_window)
    {
        ImGui::PopStyleColor(5);
    }
}

std::pair<std::vector<double>&, std::vector<double>&> PlottingWindow::get_xy_downsampled(
    sparq_dataset_t& dataset,
    std::size_t const max_samples,
    double x_min,
    double x_max)
{
    auto& d = dataset;

    auto const downsampling_enabled = (_config_handler.ini["downsampling"]["enabled"] == "1");
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
    if (_data_handler.x_fit_select == 1)
    {
        x_min = std::numeric_limits<double>::lowest();
        x_max = std::numeric_limits<double>::max();
    }

    std::vector<double>* x_values;
    switch (_data_handler.x_axis_select)
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
    auto const lower = std::lower_bound(x_values->begin(), x_values->end(), x_min);
    auto const upper = std::upper_bound(x_values->begin(), x_values->end(), x_max);

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

    auto const step = static_cast<double>(number_of_samples - 1) / (max_samples - 1);

    // Start from the second value
    auto index = step;

    for (std::size_t i = 1; i < max_samples - 1; ++i)
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

// void PlottingWindow::update_axes()
//{
//     // X Axis Setup
//     const char* x_axis_label = x_axis_types[_data_handler.x_axis_select].axis_label;
//
//     if (_data_handler.x_axis_select == 2)
//     {
//         ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
//     }
//
//     switch (_data_handler.x_fit_select)
//     {
//     // Manual
//     case 0:
//         ImPlot::SetupAxis(ImAxis_X1, x_axis_label, 0);
//         break;
//     // Fit all
//     default:
//     case 1:
//         ImPlot::SetupAxis(ImAxis_X1, x_axis_label, ImPlotAxisFlags_AutoFit);
//         break;
//     // Last N values
//     case 2:
//         config_limits_n_values();
//         break;
//     }
//
//     // Y Axis Setup
//     switch (_data_handler.y_fit_select)
//     {
//     // Manual
//     case 0:
//         ImPlot::SetupAxis(ImAxis_Y1, nullptr, 0);
//         break;
//     // Fit all
//     case 1:
//         ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);
//         break;
//     }
// }

void PlottingWindow::config_limits_n_values()
{
    switch (_data_handler.x_axis_select)
    {
    // Last N Samples
    case 0:
    {
        auto const max_sample = static_cast<std::size_t>(round(_data_handler.get_max_sample())) - 1;
        auto min_sample = max_sample - static_cast<std::size_t>(_data_handler.last_n) + 1;

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
        auto const max_rel_time = _data_handler.get_max_rel_time();
        auto min_rel_time = max_rel_time - _data_handler.last_n;

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
        auto const max_abs_time = _data_handler.get_max_abs_time();
        auto min_abs_time = max_abs_time - _data_handler.last_n;

        if (min_abs_time < 0)
        {
            min_abs_time = 0;
        }

        ImPlot::SetupAxisLimits(ImAxis_X1, min_abs_time, max_abs_time, ImPlotCond_Always);
        break;
    }
    }
}

std::vector<float> PlottingWindow::bilinear_interpolate(std::vector<float> const& original_image, int const original_rows, int const original_cols, float const scale_factor)
{
    auto const new_rows = static_cast<int>(original_rows * scale_factor);
    auto const new_cols = static_cast<int>(original_cols * scale_factor);

    std::vector<float> interpolated_image(new_rows * new_cols);

    // Helper lambda to get pixel value with bounds checking
    auto get_pixel = [&](int row, int col) -> float {
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
            auto const orig_row_f = (new_row + 0.5f) / scale_factor - 0.5f;
            auto const orig_col_f = (new_col + 0.5f) / scale_factor - 0.5f;

            // Get the four surrounding pixel coordinates
            auto const row0 = static_cast<int>(std::floor(orig_row_f));
            auto const row1 = row0 + 1;
            auto const col0 = static_cast<int>(std::floor(orig_col_f));
            auto const col1 = col0 + 1;

            // Calculate interpolation weights
            auto const row_weight = orig_row_f - row0;
            auto const col_weight = orig_col_f - col0;

            // Get the four corner pixel values
            auto const top_left = get_pixel(row0, col0);
            auto const top_right = get_pixel(row0, col1);
            auto const bottom_left = get_pixel(row1, col0);
            auto const bottom_right = get_pixel(row1, col1);

            // Perform bilinear interpolation
            auto const top_interp = top_left * (1.0f - col_weight) + top_right * col_weight;
            auto const bottom_interp = bottom_left * (1.0f - col_weight) + bottom_right * col_weight;
            auto const final_value = top_interp * (1.0f - row_weight) + bottom_interp * row_weight;

            interpolated_image[static_cast<std::size_t>(new_row) * new_cols + new_col] = final_value;
        }
    }

    return interpolated_image;
}
