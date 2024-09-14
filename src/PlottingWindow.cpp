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
        if (ImPlot::BeginPlot("Data", ImVec2(-1, -1)))
        {
            ImPlot::SetupAxes(x_axis_types[_data_handler->x_axis_select].axis_label, "", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

            ImPlotContext *ctx = ImPlot::GetCurrentContext();
            ImPlotPlot *plot = ctx->CurrentPlot;

            uint8_t i = 0;
            for (auto &ds : datasets)
            {
                std::string name = (ds.name[0] == 0) ? std::to_string(ds.id) : std::string(ds.name);
                ImPlot::SetNextLineStyle(ds.color, 3);

                std::vector<float> *x_values;

                switch (_data_handler->x_axis_select)
                {
                default:
                case 0:
                    x_values = &ds.samples;
                    break;
                case 1:
                    x_values = &ds.relative_times;
                    break;
                case 2:
                    x_values = &ds.samples;
                    break;
                }

                ImPlot::PlotLine((name + "###LP" + std::to_string(ds.id)).c_str(), (*x_values).data(), ds.y_values.data(), ds.y_values.size());

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