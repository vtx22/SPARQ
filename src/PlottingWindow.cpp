#include "PlottingWindow.hpp"

PlottingWindow::PlottingWindow(DataHandler *data_handler) : _data_handler(data_handler)
{
}

PlottingWindow::~PlottingWindow()
{
}

void PlottingWindow::update()
{
    std::lock_guard<std::mutex> lock(_data_handler->get_data_mutex());
    std::vector<sparq_dataset_t> &datasets = _data_handler->get_datasets_editable();
    if (ImGui::Begin(ICON_FA_CHART_LINE "  Plot"))
    {
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
                for (auto &ds : datasets)
                {
                    std::string name = (ds.name[0] == 0) ? std::to_string(ds.id) : std::string(ds.name);
                    ImPlot::SetNextLineStyle(ds.color, 3);

                    auto [x_values, y_values] = get_xy_values(ds);

                    if (ds.display_square)
                    {
                        ImPlot::PlotStairs((name + "###LP" + std::to_string(ds.id)).c_str(), x_values->data(), y_values->data(), y_values->size());
                    }
                    else
                    {
                        ImPlot::PlotLine((name + "###LP" + std::to_string(ds.id)).c_str(), x_values->data(), y_values->data(), y_values->size());
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

    ImGui::End();
}

std::tuple<std::vector<double> *, std::vector<double> *> PlottingWindow::get_xy_values(sparq_dataset_t &dataset)
{
    std::vector<double> *x_values, *y_values;

    y_values = &dataset.y_values;

    switch (_data_handler->x_axis_select)
    {
    default:
    case 0:
        x_values = &dataset.samples;
        break;
    case 1:
        x_values = &dataset.relative_times;
        break;
    case 2:
        x_values = &dataset.absolute_times;
        break;
    }

    return {x_values, y_values};
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