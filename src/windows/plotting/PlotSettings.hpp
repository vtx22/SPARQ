#pragma once

namespace spq::plotting::internal
{
    template <typename Enum>
    constexpr void enum_combo(const char* label, Enum& value, std::span<const char* const> const items)
    {
        int selected = static_cast<int>(value);
        if (ImGui::Combo(label, &selected, items.data(), items.size()))
        {
            value = static_cast<Enum>(selected);
        }
    }
}

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
        "Date Time"};

    constexpr std::array<char const*, static_cast<std::size_t>(x_unit_t::COUNT)> x_unit_labels{
        "Samples",
        "Relative Time [s]",
        "Date Time"};

    struct plot_settings
    {
        virtual ~plot_settings() = default;
        virtual void show_settings() = 0;
    };

    struct timeseries_settings : plot_settings
    {
        x_fit_t x_fit{x_fit_t::all};
        y_fit_t y_fit{y_fit_t::all};
        x_unit_t x_unit{};

        void show_settings() override
        {
            auto const spacing_right = 3.5f * ImGui::GetFontSize();
            ImGui::PushItemWidth(-spacing_right);

            if (ImGui::CollapsingHeader("X Axis"))
            {
                internal::enum_combo("X Unit", x_unit, x_unit_names);
                internal::enum_combo("X Fit", x_fit, x_fit_names);
            }

            if (ImGui::CollapsingHeader("Y Axis"))
            {
                internal::enum_combo("Y Fit", y_fit, y_fit_names);
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
        bool autoscale{true};
        bool invert_scale{};
        int rows{1};
        int cols{1};
        bool smooth{};
        bool equal{};
        int smoothing_factor{5};

        void show_settings() override
        {
            ImGui::SetNextItemWidth(100);
            if (ImGui::InputInt("##HMI_R", &rows))
            {
                rows = std::max(0, rows);
            }

            ImGui::SameLine();
            ImGui::Text("x");

            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            if (ImGui::InputInt("###HMI_C", &cols))
            {
                cols = std::max(0, cols);
            }

            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::Checkbox("Equal", &equal);

            ImGui::Checkbox("Values", &show_values);

            ImGui::SameLine();
            ImGui::Checkbox("Smooth", &smooth);

            ImGui::SameLine();
            if (ImGui::InputInt("Factor", &smoothing_factor))
            {
                smoothing_factor = std::max(1, smoothing_factor);
            }

            ImGui::Checkbox("Autoscale", &autoscale);
            ImGui::SameLine();
            ImGui::Checkbox("Invert", &invert_scale);
            if (!autoscale)
            {
                ImGui::InputFloat("Minimum Scale", &scale_min);
                ImGui::InputFloat("Maximum Scale", &scale_max);
            }
        }
    };
}
