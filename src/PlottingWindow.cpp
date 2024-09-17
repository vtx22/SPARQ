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
            ImPlot::SetupAxes(x_axis_types[_data_handler->x_axis_select].axis_label, "", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            if (_data_handler->x_axis_select == 2)
            {
                ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
            }

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