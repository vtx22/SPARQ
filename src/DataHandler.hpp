#pragma once

#include "ConsoleWindow.hpp"
#include "Datasets.hpp"
#include "ImGuiNotify.hpp"
#include "implot.h"
#include "serial.hpp"
#include "sparq_config.h"
#include "sparq_types.hpp"

using namespace std::chrono;

class DataHandler final
{
public:
    /**
     * @brief A RAII class that locks the data mutex and provides access to the datasets were the lock is linked to the lifetime of the object.
     * @details This class is used to ensure thread-safe access to the datasets while holding a lock on the data mutex.
     * It is returned by the datasets() function, which locks the data mutex and returns an instance of this struct.
     * The lock is released when the instance of this struct goes out of scope.
     */
    class LockedDatasets
    {
    public:
        LockedDatasets(std::mutex& m, Datasets& d)
            : m_datasets{d},
              m_lock{m}
        {
        }

        [[nodiscard]]
        constexpr auto& get() const noexcept
        {
            return m_datasets;
        }

    private:
        Datasets& m_datasets;
        std::unique_lock<std::mutex> m_lock{};
    };

    LockedDatasets datasets()
    {
        return LockedDatasets{_data_mutex, m_datasets};
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

    void update()
    {
        update_markers();
    }

    void receiver_loop();
    std::optional<sparq_message_t> receive_message();

    [[nodiscard]]
    constexpr std::vector<sparq_marker_t>& get_markers() noexcept
    {
        return _markers;
    }

    void update_markers();

    uint8_t x_axis_select = 0;
    uint8_t x_fit_select = 1;
    uint8_t y_fit_select = 1;

    int last_n = 10;

    void export_data_csv(Datasets& datasets);

    [[nodiscard]]
    constexpr std::mutex& get_serial_mutex() noexcept
    {
        return _serial_mutex;
    }

private:
    void handle_command(sparq_message_t const& message);

    Serial& _sp;
    ConsoleWindow& _console_window;

    std::vector<uint8_t> _serial_buffer;
    std::vector<uint8_t> _message_buffer;

    std::vector<sparq_marker_t> _markers;

    std::thread _receive_thread;
    std::atomic<bool> _running = true;
    std::mutex _data_mutex, _serial_mutex;

    Datasets m_datasets{};
};
