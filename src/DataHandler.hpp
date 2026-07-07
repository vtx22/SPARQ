#pragma once

#include "ConsoleWindow.hpp"
#include "Datasets.hpp"
#include "ImGuiNotify.hpp"
#include "implot.h"
#include "serial.hpp"
#include "sparq_config.h"
#include "sparq_types.hpp"

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
        return LockedDatasets{m_data_mutex, m_datasets};
    }

    DataHandler(Serial& sp, ConsoleWindow& console_window)
        : m_sp(sp),
          m_console_window(console_window)
    {
        m_sp.set_timeouts(0xFFFF'FFFF, 0, 0, 0, 0);
        m_serial_buffer.reserve(static_cast<std::size_t>(SPARQ_MAX_MESSAGE_LENGTH) * 2);

        m_receive_thread = std::thread(&DataHandler::receiver_loop, this);
        std::cout << "Starting receiver thread ...\n";
    }

    ~DataHandler()
    {
        m_running = false;
        if (m_receive_thread.joinable())
        {
            m_receive_thread.join();
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
        return m_markers;
    }

    void update_markers();

    uint8_t x_axis_select = 0;
    uint8_t x_fit_select = 1;
    uint8_t y_fit_select = 1;

    int last_n = 10;

    static void export_datasets_csv(Datasets const& datasets);

    [[nodiscard]]
    constexpr std::mutex& get_serial_mutex() noexcept
    {
        return m_serial_mutex;
    }

private:
    void handle_command(sparq_message_t const& message);

    Serial& m_sp;
    ConsoleWindow& m_console_window;

    std::vector<uint8_t> m_serial_buffer;
    std::vector<uint8_t> m_message_buffer;

    std::vector<sparq_marker_t> m_markers;

    std::thread m_receive_thread;
    std::atomic<bool> m_running = true;
    std::mutex m_data_mutex, m_serial_mutex;

    Datasets m_datasets{};
};
