#pragma once

#include "../Window.hpp"
#include "../sparq_types.hpp"
#include "implot.h"
#include "implot_internal.h"

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
              {},
              data_handler),
          m_id(id)
    {
        update_window_name();
    }

    void config_limits_n_values() const;

    constexpr void show_highlighting_rectangle() const;

    std::vector<float> bilinear_interpolate(std::vector<float> const& original_image, int original_rows, int original_cols, float scale_factor);

    std::pair<std::vector<double>&, std::vector<double>&> get_xy_downsampled(sparq_dataset_t& dataset, std::size_t max_samples, double x_min, double x_max);

    [[nodiscard]]
    spq::plotting::plot_settings_t& settings() noexcept
    {
        return m_plot_settings;
    }

    [[nodiscard]]
    constexpr auto id() const noexcept
    {
        return m_id;
    }

    constexpr void set_highlighted(bool const highlight) noexcept
    {
        m_highlight_window = highlight;
    }

private:
    [[nodiscard]]
    constexpr ImPlotFlags get_plot_flags() const noexcept
    {
        ImPlotFlags plot_flags = ImPlotFlags_NoMenus;

        if (m_plot_settings.type == spq::plotting::plot_type_t::heatmap && m_plot_settings.equal)
        {
            plot_flags |= ImPlotFlags_Equal;
        }

        return plot_flags;
    }

    void update_plot_contents(Datasets& datasets);
    void update_markers() const;

    void handle_plot_timeseries(Datasets& datasets);
    void handle_plot_xy(Datasets& datasets);
    void handle_plot_single_value(Datasets& datasets);
    void handle_plot_heatmap(Datasets const& datasets);

    void update_window_name()
    {
        using namespace spq::plotting;
        m_window_name = internal::window_name_prefix;
        m_window_name += plot_type_names.at(static_cast<uint8_t>(m_plot_settings.type));
        m_window_name += internal::window_name_id_prefix;
        m_window_name += std::to_string(m_id);
    }

    std::vector<double> _x_downsampled, _x_in_view;
    std::vector<double> _y_downsampled, _y_in_view;

    bool m_highlight_window = false;
    bool m_highlight_colors_pushed = false;
    spq::plotting::plot_settings_t m_plot_settings;
    std::size_t m_id{};

protected:
    void update_content(Datasets& datasets) override;

    void before_imgui_begin() override
    {
        // Add highlight styles. Keep in sync with after_imgui_end()
        m_highlight_colors_pushed = m_highlight_window;
        if (m_highlight_colors_pushed)
        {
            constexpr auto color = spq::styling::plot_highlight_color;
            ImGui::PushStyleColor(ImGuiCol_TitleBg, color);
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive, color);
            ImGui::PushStyleColor(ImGuiCol_Tab, color);
            ImGui::PushStyleColor(ImGuiCol_TabActive, color);
            ImGui::PushStyleColor(ImGuiCol_TabHovered, color);
            ImGui::PushStyleColor(ImGuiCol_TabUnfocused, color);
            ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, color);
        }
    }

    void after_imgui_end() override
    {
        // Remove highlight styles. Keep in sync with before_imgui_begin()
        if (m_highlight_colors_pushed)
        {
            ImGui::PopStyleColor(7);
        }
    }

    [[nodiscard]]
    constexpr bool has_close_button() const noexcept override
    {
        return true;
    }
};
