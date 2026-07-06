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
}

class PlottingWindow final : public Window
{
public:
    PlottingWindow(DataHandler& data_handler, std::size_t const id)
        : Window(
              std::string(spq::plotting::internal::window_name_prefix)
                  + spq::plotting::internal::window_name_id_prefix
                  + std::to_string(id),
              data_handler),
          _id(id)
    {
    }

    void config_limits_n_values() const;

    constexpr void show_highlighting_rectangle() const;

    std::vector<float> bilinear_interpolate(std::vector<float> const& original_image, int original_rows, int original_cols, float scale_factor);

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

    constexpr void set_highlighted(bool const highlight) noexcept
    {
        _highlight_window = highlight;
    }

private:
    ImPlotFlags get_plot_flags() const;
    void update_plot_contents();
    void update_markers() const;

    void handle_plot_timeseries(std::vector<sparq_dataset_t>& datasets);
    void handle_plot_xy(std::vector<sparq_dataset_t>& datasets);
    void handle_plot_single_value(std::vector<sparq_dataset_t>& datasets);
    void handle_plot_heatmap(std::vector<sparq_dataset_t> const& datasets);

    void update_window_name();

    std::vector<double> _x_downsampled, _x_in_view;
    std::vector<double> _y_downsampled, _y_in_view;

    bool _highlight_window = false;
    bool _highlight_colors_pushed = false;
    spq::plotting::plot_settings _plot_settings;
    std::size_t _id{};

protected:
    void update_content() override;
    void before_imgui_begin() override;
    void after_imgui_end() override;

    [[nodiscard]]
    constexpr bool has_close_button() const noexcept override
    {
        return true;
    }
};
