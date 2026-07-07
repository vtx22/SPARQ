#pragma once

#include "ConsoleWindow.hpp"
#include "ImGuiNotify.hpp"
#include "implot.h"
#include "serial.hpp"
#include "sparq_config.h"
#include "sparq_types.hpp"

using namespace std::chrono;

class Datasets final
{
public:
    /**
     * @brief Shows all datasets.
     * @details This function sets the show flag of all datasets to true, making them visible in all plots.
     */
    constexpr void show_all() noexcept
    {
        for (auto& ds : m_datasets)
        {
            ds.show = true;
        }
    }

    /**
     * @brief Hides all datasets.
     * @details This function sets the hide flag of all datasets to true, making them invisible in all plots.
     */
    constexpr void hide_all() noexcept
    {
        for (auto& ds : m_datasets)
        {
            ds.hide = true;
        }
    }

    /**
     * @brief Gets a reference to the vector of datasets.
     * @return A reference to the vector of datasets.
     */
    [[nodiscard]]
    constexpr auto& data() noexcept
    {
        return m_datasets;
    }

    /**
     * @brief Checks if the vector of datasets is empty.
     * @return True if the vector of datasets is empty, false otherwise.
     */
    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
        return m_datasets.empty();
    }

    /**
     * @brief Gets the number of stored datasets.
     * @return The number of stored datasets.
     */
    [[nodiscard]]
    constexpr auto size() const noexcept
    {
        return m_datasets.size();
    }

    constexpr auto& operator[](std::size_t const index) noexcept
    {
        return m_datasets[index];
    }

    constexpr auto const& operator[](std::size_t const index) const noexcept
    {
        return m_datasets[index];
    }

    /**
     * @brief Gets a reference to a dataset with the given ID.
     * @param id The ID of the dataset to get.
     * @return An optional reference to the dataset, or std::nullopt if no dataset with the given ID was found.
     */
    constexpr std::optional<std::reference_wrapper<sparq_dataset_t>> get(std::size_t const id) noexcept
    {
        for (auto& ds : m_datasets)
        {
            if (ds.id == id)
            {
                return ds;
            }
        }

        return std::nullopt;
    }

    /**
     * @brief Checks if a dataset with the given ID exists.
     * @param id The ID of the dataset to check.
     * @return True if a dataset with the given ID exists, false otherwise.
     */
    constexpr bool exists(std::size_t const id) noexcept
    {
        return get(id).has_value();
    }

    /**
     * @brief Adds a new dataset to the list of datasets.
     * @attention If a dataset with the same ID already exists, the new dataset will not be added.
     * @param dataset The dataset to add.
     * @return True if the dataset was added, false if a dataset with the same ID already exists.
     */
    constexpr bool add_dataset(sparq_dataset_t const& dataset)
    {
        if (exists(dataset.id))
        {
            return false;
        }

        m_datasets.emplace_back(dataset);
        return true;
    }

    /**
     * @brief Clears the data of all datasets.
     */
    void clear_all()
    {
        std::cout << "Clearing all datasets ...\n";

        for (auto const& d : m_datasets)
        {
            clear(d.id);
        }
    }

    /**
     * @brief Clears the data of a dataset with the given ID.
     * @param id The ID of the dataset to clear.
     * @return True if the dataset was cleared, false if no dataset with the given ID was found.
     */
    bool clear(std::size_t const id)
    {
        auto const dataset = get(id);

        if (!dataset.has_value())
        {
            return false;
        }

        dataset->get().clear();

        // If all datasets are now empty, reset start time
        for (auto const& ds : m_datasets)
        {
            if (!ds.samples.empty())
            {
                return true;
            }
        }

        m_current_absolute_sample = 0;
        m_first_receive_timestamp = 0;
        m_timestamps.clear();
        m_rel_times.clear();

        return true;
    }

    /**
     * @brief Deletes a dataset with the given ID from the list of datasets.
     * @param id The ID of the dataset to delete.
     * @return True if the dataset was deleted, false if no dataset with the given ID was found.
     */
    bool delete_dataset(std::size_t const id)
    {
        for (std::size_t i = 0; i < m_datasets.size(); i++)
        {
            if (m_datasets[i].id == id)
            {
                m_datasets.erase(m_datasets.begin() + i);

                if (m_datasets.empty())
                {
                    m_first_receive_timestamp = 0;
                    m_current_absolute_sample = 0;
                }

                return true;
            }
        }

        return false;
    }

    /**
     * @brief Deletes all datasets from the list of datasets.
     */
    void delete_all()
    {
        std::cout << "Deleting all datasets ...\n";

        m_datasets.clear();
        m_first_receive_timestamp = 0;
        m_current_absolute_sample = 0;
    }

    /**
     * @brief Adds data from a received message to the appropriate dataset.
     * @param message The received message containing the data to add.
     */
    void add_from_message(sparq_message_t const& message)
    {
        if (m_first_receive_timestamp == 0)
        {
            m_first_receive_timestamp = message.timestamp;
        }

        m_timestamps.emplace_back(message.timestamp);
        m_rel_times.emplace_back((message.timestamp - m_first_receive_timestamp) / 1000.0);

        // if (!_console_window.TextOnly)
        // {
        //     _console_window.add_data_to_log(message.ids.data(), message.values.data(), message.nval);
        // }

        for (uint16_t i = 0; i < message.nval; i++)
        {
            sparq_dataset_t* ds = nullptr;

            for (auto& d : m_datasets)
            {
                if (d.id == message.ids[i])
                {
                    ds = &d;
                    break;
                }
            }

            // The dataset does not exist, we have to create a new one
            if (ds == nullptr)
            {
                std::cout << "DS not found, creating new one! ID: " << static_cast<int>(message.ids[i]) << "\n";

                sparq_dataset_t ds_new;

                ds_new.id = message.ids[i];
                ds_new.color = ImPlot::GetColormapColor(ImPlot::GetColormapSize() / 2 + m_datasets.size());

                auto const rel_time = (message.timestamp - m_first_receive_timestamp) / 1000.0;
                auto const abs_time = message.timestamp / 1000.0;

                ds_new.append_raw_values(m_current_absolute_sample, rel_time, abs_time, message.values[i]);
                m_datasets.push_back(ds_new);

                continue;
            }

            // Dataset already exists but might be empty
            auto const new_sample = ds->samples.empty()
                                      ? static_cast<double>(m_current_absolute_sample)
                                      : (ds->samples.back() + 1.0);
            auto const new_rel_time = (message.timestamp - m_first_receive_timestamp) / 1000.0;
            auto const new_abs_time = message.timestamp / 1000.0;
            auto const new_y_value = message.values[i];

            ds->append_raw_values(new_sample, new_rel_time, new_abs_time, new_y_value);
        }

        m_current_absolute_sample++;
    }

    /**
     * @brief Gets the maximum sample number across all datasets.
     * @return The maximum sample number as a double.
     */
    [[nodiscard]]
    constexpr auto get_max_sample() const noexcept
    {
        return static_cast<double>(m_current_absolute_sample);
    }

    /**
     * @brief Gets the maximum relative time across all datasets.
     * @return The maximum relative time as a double.
     */
    [[nodiscard]]
    constexpr double get_max_rel_time() const noexcept
    {
        if (m_datasets.empty())
        {
            return 0.0;
        }

        return std::ranges::max(
            m_datasets | std::views::transform([](auto const& ds) {
                return ds.relative_times.back();
            }));
    }

    /**
     * @brief Gets the maximum absolute time across all datasets.
     * @return The maximum absolute time.
     */
    [[nodiscard]]
    constexpr double get_max_abs_time() const noexcept
    {
        if (m_datasets.empty())
        {
            return 0.0;
        }

        return std::ranges::max(
            m_datasets | std::views::transform([](auto const& ds) {
                return ds.absolute_times.back();
            }));
    }

    /**
     * @brief Gets the current absolute sample number.
     * @return The current absolute sample number.
     */
    [[nodiscard]]
    constexpr auto get_current_absolute_sample() const noexcept
    {
        return m_current_absolute_sample;
    }

    /**
     * @brief Gets the timestamp of the first received message.
     * @return The timestamp of the first received message.
     */
    [[nodiscard]]
    constexpr auto get_first_receive_timestamp() const noexcept
    {
        return m_first_receive_timestamp;
    }

    /**
     * @brief Gets a reference to the vector of timestamps.
     * @return A reference to the vector of timestamps.
     */
    [[nodiscard]]
    constexpr auto const& get_timestamps() const noexcept
    {
        return m_timestamps;
    }

    /**
     * @brief Gets a reference to the vector of relative times.
     * @return A reference to the vector of relative times.
     */
    [[nodiscard]]
    constexpr auto const& get_relative_times() const noexcept
    {
        return m_rel_times;
    }

private:
    std::vector<sparq_dataset_t> m_datasets;

    std::size_t m_current_absolute_sample{};
    uint64_t m_first_receive_timestamp{};
    std::vector<uint64_t> m_timestamps;
    std::vector<float> m_rel_times;
};

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
