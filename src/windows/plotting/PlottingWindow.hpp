#pragma once

#include "../../sparq_types.hpp"
#include "../Window.hpp"
#include "implot.h"
#include "implot_internal.h"

namespace spq::styling
{
    constexpr auto plot_highlight_color = IM_COL32(0, 102, 255, 255);
    constexpr auto plot_highlight_border_width = 3.f;
}

namespace spq::ui
{
    class PlottingWindow : public Window
    {
    public:
        PlottingWindow(std::string const& name, data::DataHandler& data_handler, std::size_t const id)
            : Window(
                  name,
                  data_handler),
              m_id{id}
        {
        }

        constexpr void show_highlighting_rectangle() const
        {
            if (!m_highlight_window)
            {
                return;
            }

            auto constexpr border_width = styling::plot_highlight_border_width;
            auto p0 = ImGui::GetWindowPos();
            auto const sz = ImGui::GetWindowSize();
            ImVec2 p1{p0.x + sz.x, p0.y + sz.y};

            p0.x += border_width;
            p0.y += border_width;
            p1.x -= border_width;
            p1.y -= border_width;

            if (ImGui::IsWindowDocked())
            {
                p0.y += ImGui::GetFrameHeight();
            }

            ImGui::GetWindowDrawList()->AddRect(
                p0,
                p1,
                styling::plot_highlight_color,
                ImGui::GetStyle().WindowRounding,
                0,
                border_width);
        }

        [[nodiscard]]
        auto& settings() noexcept
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

            if (m_plot_settings.type == spq::ui::plot_type_t::heatmap && m_plot_settings.equal)
            {
                plot_flags |= ImPlotFlags_Equal;
            }

            return plot_flags;
        }

        void update_plot_contents(data::Datasets& datasets)
        {
            if (ImPlot::BeginPlot("##Plot", ImVec2(-1, -1), get_plot_flags()))
            {
                ImPlot::EndPlot();
            }
        }

        bool m_highlight_window = false;
        bool m_highlight_colors_pushed = false;
        plot_settings_t m_plot_settings;
        std::size_t const m_id{};

    protected:
        void update_content(data::Datasets& datasets) override
        {
            update_plot_contents(datasets);
            show_highlighting_rectangle();
        }

        void before_imgui_begin() override
        {
            // Add highlight styles. Keep in sync with after_imgui_end()
            m_highlight_colors_pushed = m_highlight_window;
            if (m_highlight_colors_pushed)
            {
                constexpr auto color = styling::plot_highlight_color;
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
}
