#pragma once

#include "ConsoleWindow.hpp"
#include "ImGuiNotify.hpp"
#include "implot.h"
#include "serial.hpp"
#include "sparq_config.h"
#include "sparq_types.hpp"

using namespace std::chrono;

class DataHandler
{
public:
    /**
     * @brief A RAII class that locks the data mutex and provides access to the datasets.
     * @details This struct is used to ensure thread-safe access to the datasets while holding a lock on the data mutex.
     * It is returned by the datasets() function, which locks the data mutex and returns an instance of this struct.
     */
    class LockedDatasets
    {
    public:
        LockedDatasets(std::mutex& m, std::vector<sparq_dataset_t>& d)
            : datasets{d},
              lock{m}
        {
        }

        [[nodiscard]]
        constexpr auto& get() const noexcept
        {
            return datasets;
        }

    private:
        std::vector<sparq_dataset_t>& datasets;
        std::unique_lock<std::mutex> lock{};
    };

    LockedDatasets datasets()
    {
        return LockedDatasets{_data_mutex, _datasets};
    }

    DataHandler(Serial& sp, ConsoleWindow& console_window)
        : _sp(sp),
          _console_window(console_window)
    {
        _sp.set_timeouts(0xFFFF'FFFF, 0, 0, 0, 0);
        _serial_buffer.reserve(static_cast<std::size_t>(SPARQ_MAX_MESSAGE_LENGTH) * 2);

        _receive_thread = std::thread(&DataHandler::receiver_loop, this);
        std::cout << "Starting receiver thread ...\n";
    }

    ~DataHandler()
    {
        _running = false;
        if (_receive_thread.joinable())
        {
            _receive_thread.join();
        }
    }

    void update();
    void receiver_loop();
    sparq_message_t receive_message();

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

    void export_data_csv();

    [[nodiscard]]
    auto get_max_sample()
    {
        std::lock_guard lock{_data_mutex};
        return static_cast<double>(current_absolute_sample);
    }

    [[nodiscard]]
    double get_max_rel_time()
    {
        std::lock_guard lock{_data_mutex};

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
    double get_max_abs_time()
    {
        std::lock_guard lock{_data_mutex};

        if (_datasets.empty())
        {
            return 0.0;
        }

        return std::ranges::max(
            _datasets | std::views::transform([](auto const& ds) {
                return ds.absolute_times.back();
            }));
    }

    [[nodiscard]]
    constexpr std::mutex& get_serial_mutex() noexcept
    {
        return _serial_mutex;
    }

    [[nodiscard]]
    bool no_datasets()
    {
        std::lock_guard lock{_data_mutex};
        return _datasets.empty();
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

    void add_to_datasets(sparq_message_t const& message);
    void handle_command(sparq_message_t const& message);

    std::thread _receive_thread;
    std::atomic<bool> _running = true;
    std::mutex _data_mutex, _serial_mutex;
};
