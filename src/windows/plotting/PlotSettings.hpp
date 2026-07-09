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

    struct plot_settings
    {
        virtual ~plot_settings() = default;
        virtual void show_settings() = 0;
    };

    struct timeseries_settings : plot_settings
    {
        void show_settings() override
        {
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
