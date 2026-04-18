#pragma once

#include "Window.hpp"
#include "implot.h"
#include "implot_internal.h"
#include "sparq_types.hpp"

class PlottingWindow : public Window
{
public:
    PlottingWindow(DataHandler& data_handler, uint8_t id)
        : Window(std::string(ICON_FA_CHART_LINE "  Plot##") + std::to_string(id), data_handler)
    {
    }

    void update_content() override;
    void before_imgui_begin() override;
    void after_imgui_end() override;
    void update_axes();
    void config_limits_n_values();

    constexpr void show_highlighting_rectangle() const;

    std::vector<float> bilinear_interpolate(const std::vector<float>& original_image, int original_rows, int original_cols, float scale_factor);

    std::pair<std::vector<double>&, std::vector<double>&> get_xy_downsampled(sparq_dataset_t& dataset, std::size_t max_samples, double x_min, double x_max);

private:
    bool _highlight_window = false;
    bool _in_focus_flag = false;

    std::vector<double> _x_downsampled, _x_in_view;
    std::vector<double> _y_downsampled, _y_in_view;
};
