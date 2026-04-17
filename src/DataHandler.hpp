#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <optional>
#include <ranges>
#include <thread>

#include "implot.h"
#include "serial.hpp"
#include "sparq_config.h"
#include "sparq_types.hpp"

#include "ImGuiNotify.hpp"

#include "ConsoleWindow.hpp"

using namespace std::chrono;

class DataHandler
{
public:
    DataHandler(Serial& sp, ConsoleWindow& console_window);
    ~DataHandler();

    void update();
    void receiver_loop();
    sparq_message_t receive_message();

    [[nodiscard]]
    constexpr std::vector<sparq_dataset_t> const& get_datasets() const noexcept
    {
        return _datasets;
    }

    [[nodiscard]]
    constexpr std::vector<sparq_dataset_t>& get_datasets_editable() noexcept
    {
        return _datasets;
    }

    [[nodiscard]]
    constexpr std::vector<sparq_marker_t>& get_markers() noexcept
    {
        return _markers;
    }

    void update_markers();

    bool add_dataset(sparq_dataset_t const& dataset);
    bool delete_dataset(uint8_t id);
    void delete_all_datasets();

    bool clear_dataset(uint8_t id);
    void clear_all_datasets();

    void hide_all_datasets();
    void show_all_datasets();

    std::optional<std::reference_wrapper<sparq_dataset_t>> get_dataset(uint8_t id);

    uint8_t x_axis_select = 0;
    uint8_t x_fit_select = 1;
    uint8_t y_fit_select = 1;

    int last_n = 10;

    void export_data_csv() const;

    [[nodiscard]]
    auto get_max_sample()
    {
        std::lock_guard<std::mutex> lock(_data_mutex);
        return static_cast<double>(current_absolute_sample);
    }

    [[nodiscard]]
    auto get_max_rel_time()
    {
        std::lock_guard<std::mutex> lock(_data_mutex);

        if (_datasets.empty())
        {
            return 0.0;
        }

        return std::ranges::max(
            _datasets | std::views::transform([](auto const& ds) {
                return ds.relative_times.back();
            }));
    }

    [[nodiscard]]
    auto get_max_abs_time()
    {
        std::lock_guard<std::mutex> lock(_data_mutex);

        if (_datasets.empty())
        {
            return 0.0;
        }

        return std::ranges::max(
            _datasets | std::views::transform([](auto const& ds) {
                return ds.absolute_times.back();
            }));
    }

    sparq_plot_settings_t plot_settings;

    [[nodiscard]]
    constexpr std::mutex& get_data_mutex() noexcept
    {
        return _data_mutex;
    }

    [[nodiscard]]
    constexpr std::mutex& get_serial_mutex() noexcept
    {
        return _serial_mutex;
    }

private:
    uint32_t current_absolute_sample = 0;
    uint64_t first_receive_timestamp = 0;

    Serial& _sp;
    ConsoleWindow& _console_window;

    std::vector<uint8_t> _serial_buffer;
    std::vector<uint8_t> _message_buffer;

    std::vector<sparq_dataset_t> _datasets;
    std::vector<uint64_t> _timestamps;
    std::vector<float> _rel_times;

    std::vector<sparq_marker_t> _markers;

    void add_to_datasets(const sparq_message_t& message);
    void handle_command(const sparq_message_t& message);

    std::thread _receive_thread;
    std::atomic<bool> _running = true;
    std::mutex _data_mutex, _serial_mutex;
};
