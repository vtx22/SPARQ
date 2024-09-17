#include "PlottingWindow.hpp"

PlottingWindow::PlottingWindow(DataHandler *data_handler) : _data_handler(data_handler)
{
}

PlottingWindow::~PlottingWindow()
{
}

void PlottingWindow::update()
{
    std::vector<sparq_dataset_t> &datasets = _data_handler->get_datasets_editable();
    if (ImGui::Begin("Plot"))
    {
        if (ImPlot::BeginPlot("##Data", ImVec2(-1, -1)))
        {
            update_axes();

            ImPlotContext *ctx = ImPlot::GetCurrentContext();
            ImPlotPlot *plot = ctx->CurrentPlot;

            uint8_t i = 0;
            for (auto &ds : datasets)
            {
                std::string name = (ds.name[0] == 0) ? std::to_string(ds.id) : std::string(ds.name);
                ImPlot::SetNextLineStyle(ds.color, 3);

                auto [x_values, y_values] = get_xy_values(ds);

                ImPlot::PlotLine((name + "###LP" + std::to_string(ds.id)).c_str(), x_values->data(), y_values->data(), y_values->size());

                ImPlotItem *item = plot->Items.GetLegendItem(i);
                if (ds.toggle_visibility)
                {
                    item->Show = !item->Show;
                    ds.toggle_visibility = false;
                }

                ds.hidden = !item->Show;

                i++;
            }

            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}

std::tuple<std::vector<double> *, std::vector<double> *> PlottingWindow::get_xy_values(sparq_dataset_t &dataset)
{
    std::vector<double> *x_values, *y_values;

    y_values = _data_handler->interpolation ? &dataset.y_values_ip : &dataset.y_values;

    switch (_data_handler->x_axis_select)
    {
    default:
    case 0:
        x_values = _data_handler->interpolation ? &dataset.samples_ip : &dataset.samples;
        break;
    case 1:
        x_values = _data_handler->interpolation ? &dataset.relative_times_ip : &dataset.relative_times;
        break;
    case 2:
        x_values = _data_handler->interpolation ? &dataset.absolute_times_ip : &dataset.absolute_times;
        break;
    }

    return {x_values, y_values};
}

void PlottingWindow::update_axes()
{

    // X Axis Setup
    const char *x_axis_label = x_axis_types[_data_handler->x_axis_select].axis_label;
    ImPlot::SetupAxis(ImAxis_X1, x_axis_label, ImPlotAxisFlags_AutoFit);

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
        int min_sample = max_sample - _last_n;

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
        double min_rel_time = max_rel_time - _last_n;

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
        double min_abs_time = max_abs_time - _last_n;

        if (min_abs_time < 0)
        {
            min_abs_time = 0;
        }

        ImPlot::SetupAxisLimits(ImAxis_X1, min_abs_time, max_abs_time, ImPlotCond_Always);
        break;
    }
    }
}