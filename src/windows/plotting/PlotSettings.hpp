#pragma once

namespace spq::plotting
{
    enum class plot_type : uint8_t
    {
        timeseries,
        xy,
        single_value,
        bar,
        pie,
        heatmap,
        fft,
        COUNT
    };

    constexpr std::array<char const*, static_cast<std::size_t>(plot_type::COUNT)> plot_type_names{
        "Timeseries",
        "XY",
        "Single Value",
        "Bar Chart",
        "Pie Chart",
        "Heatmap",
        "FFT"};

    enum class x_fit_t : uint8_t
    {
        manual,
        all,
        last_n,
        COUNT
    };

    constexpr std::array<char const*, static_cast<std::size_t>(x_fit_t::COUNT)> x_fit_names{
        "Manual",
        "All",
        "Last N"};

    enum class y_fit_t : uint8_t
    {
        manual,
        all,
        COUNT
    };

    constexpr std::array<char const*, static_cast<std::size_t>(y_fit_t::COUNT)> y_fit_names{
        "Manual",
        "All"};

    enum class x_unit_t : uint8_t
    {
        samples,
        relative_time,
        absolute_time,
        COUNT
    };

    constexpr std::array<char const*, static_cast<std::size_t>(x_unit_t::COUNT)> x_unit_names{
        "Samples",
        "Relative Time",
        "Absolute Time"};

    struct plot_settings
    {
        virtual ~plot_settings() = default;
        virtual void show_settings() = 0;
    };

    struct timeseries_settings : plot_settings
    {
        x_fit_t x_fit{};
        y_fit_t y_fit{};
        x_unit_t x_unit{};

        void show_settings() override
        {
            auto const spacing_right = 3.5f * ImGui::GetFontSize();
            ImGui::PushItemWidth(-spacing_right);

            if (ImGui::CollapsingHeader("X Axis"))
            {
                auto selected_x_fit_type = static_cast<int>(x_fit);
                if (ImGui::Combo("X Fit", &selected_x_fit_type, x_fit_names.data(), x_fit_names.size()))
                {
                    x_fit = static_cast<x_fit_t>(selected_x_fit_type);
                }
            }

            if (ImGui::CollapsingHeader("Y Axis"))
            {
                auto selected_y_fit_type = static_cast<int>(y_fit);
                if (ImGui::Combo("Y Fit", &selected_y_fit_type, y_fit_names.data(), y_fit_names.size()))
                {
                    y_fit = static_cast<y_fit_t>(selected_y_fit_type);
                }
            }

            ImGui::PopItemWidth();
        }
    };

    struct heatmap_settings : plot_settings
    {
        bool normalize_xy{};
        bool show_values{};
        float scale_min{};
        float scale_max{100.f};
        bool autoscale{};
        bool invert_scale{};
        int rows{1};
        int cols{1};
        bool smooth{};
        int smoothing_factor{5};

        void show_settings() override
        {
        }
    };
}
