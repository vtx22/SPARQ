#pragma once

#include "Window.hpp"
#include "implot.h"
#include "implot_internal.h"
#include "sparq_types.hpp"

namespace spq::styling
{
    constexpr auto plot_highlight_color = IM_COL32(0, 102, 255, 255);
    constexpr auto plot_highlight_border_width = 3.f;
}

namespace spq::plotting::internal
{
    constexpr auto window_name_prefix = (ICON_FA_CHART_LINE "  Plot - ");
    constexpr auto window_name_id_prefix = "###PlottingWindow";
    constexpr auto invalid_plot_id = std::numeric_limits<std::size_t>::max();
}

class PlottingWindow : public Window
{
public:
    using SelectCallback = std::function<void(std::size_t)>;

    PlottingWindow(DataHandler& data_handler, std::size_t id, SelectCallback on_selected)
        : _id(id),
          _on_selected(std::move(on_selected)),
          Window(
              std::string(spq::plotting::internal::window_name_prefix)
                  + spq::plotting::internal::window_name_id_prefix
                  + std::to_string(id),
              data_handler)
    {
    }

    void update_content() override;
    void before_imgui_begin() override;
    void after_imgui_end() override;

    [[nodiscard]]
    constexpr bool has_close_button() const noexcept override
    {
        return true;
    }

    // void update_axes();
    void config_limits_n_values();

    constexpr void show_highlighting_rectangle() const;

    std::vector<float> bilinear_interpolate(const std::vector<float>& original_image, int original_rows, int original_cols, float scale_factor);

    std::pair<std::vector<double>&, std::vector<double>&> get_xy_downsampled(sparq_dataset_t& dataset, std::size_t max_samples, double x_min, double x_max);

    [[nodiscard]]
    spq::plotting::plot_settings& settings() noexcept
    {
        return _plot_settings;
    }

    [[nodiscard]]
    constexpr auto id() const noexcept
    {
        return _id;
    }

    constexpr void set_selected(bool selected) noexcept
    {
        _highlight_window = selected;
    }

private:
    ImPlotFlags get_plot_flags();
    void update_plot_contents();
    void update_markers();

    void handle_plot_timeseries();
    void handle_plot_xy();
    void handle_plot_single_value();
    void handle_plot_heatmap();

    void update_window_name();

    std::vector<double> _x_downsampled, _x_in_view;
    std::vector<double> _y_downsampled, _y_in_view;

    bool _highlight_window = false;
    SelectCallback _on_selected;
    spq::plotting::plot_settings _plot_settings;
    std::size_t _id{};
};
