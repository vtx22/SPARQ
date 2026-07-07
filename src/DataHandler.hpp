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
     * @details This function sets the show flag of all datasets to true, making them visible in the plot.
     */
    constexpr void show_all() noexcept
    {
        for (auto& ds : datasets)
        {
            ds.show = true;
        }
    }

    constexpr void hide_all() noexcept
    {
        for (auto& ds : datasets)
        {
            ds.hide = true;
        }
    }

    [[nodiscard]]
    constexpr auto& data() noexcept
    {
        return datasets;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
        return datasets.empty();
    }

    [[nodiscard]]
    constexpr auto size() const noexcept
    {
        return datasets.size();
    }

    constexpr auto& operator[](std::size_t const index) noexcept
    {
        return datasets[index];
    }

    /**
     * @brief Gets a reference to a dataset with the given ID.
     * @param id The ID of the dataset to get.
     * @return An optional reference to the dataset, or std::nullopt if no dataset with the given ID was found.
     */
    constexpr std::optional<std::reference_wrapper<sparq_dataset_t>> get(std::size_t const id) noexcept
    {
        for (auto& ds : datasets)
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

        datasets.emplace_back(dataset);
        return true;
    }

private:
    std::vector<sparq_dataset_t> datasets;
};

class DataHandler final
{
public:
    /**
     * @brief A RAII class that locks the data mutex and provides access to the datasets.
     * @details This class is used to ensure thread-safe access to the datasets while holding a lock on the data mutex.
     * It is returned by the datasets() function, which locks the data mutex and returns an instance of this struct.
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

        void unlock()
        {
            m_lock.unlock();
        }

        void lock()
        {
            m_lock.lock();
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

    /**
     * @brief Deletes a dataset with the given ID from the list of datasets.
     * @attention This function locks the data mutex!
     * @param id The ID of the dataset to delete.
     * @return True if the dataset was deleted, false if no dataset with the given ID was found.
     */
    bool delete_dataset(uint8_t const id)
    {
        std::scoped_lock lock{_data_mutex};

        for (std::size_t i = 0; i < m_datasets.size(); i++)
        {
            if (m_datasets[i].id == id)
            {
                m_datasets.data().erase(m_datasets.data().begin() + i);

                if (m_datasets.empty())
                {
                    first_receive_timestamp = 0;
                    current_absolute_sample = 0;
                }

                return true;
            }
        }

        return false;
    }

    /**
     * @brief Deletes all datasets from the list of datasets.
     * @attention This function locks the data mutex!
     */
    void delete_all_datasets()
    {
        std::cout << "Deleting all datasets ...\n";
        std::unique_lock lock(_data_mutex);

        std::vector<uint8_t> ids(m_datasets.size());

        for (std::size_t i = 0; i < m_datasets.size(); i++)
        {
            ids[i] = m_datasets[i].id;
        }

        lock.unlock();
        for (auto const& id : ids)
        {
            delete_dataset(id);
        }
    }

    /**
     * @brief Clears the data of a dataset with the given ID.
     * @attention This function locks the data mutex!
     * @param id The ID of the dataset to clear.
     * @return True if the dataset was cleared, false if no dataset with the given ID was found.
     */
    bool clear_dataset(uint8_t const id)
    {
        std::scoped_lock lock{_data_mutex};

        bool ds_found = false;
        for (auto& ds : m_datasets.data())
        {
            if (ds.id == id)
            {
                ds.clear();
                ds_found = true;
                break;
            }
        }

        if (!ds_found)
        {
            return false;
        }

        // If all datasets are now empty, reset start time
        for (auto const& ds : m_datasets.data())
        {
            if (!ds.samples.empty())
            {
                return true;
            }
        }

        first_receive_timestamp = 0;
        current_absolute_sample = 0;
        _timestamps.clear();
        _rel_times.clear();

        return true;
    }

    /**
     * @brief Clears the data of all datasets.
     * @attention This function locks the data mutex!
     */
    void clear_all_datasets()
    {
        std::cout << "Clearing all datasets ...\n";
        std::unique_lock lock(_data_mutex);

        std::vector<uint8_t> ids(m_datasets.size());

        for (std::size_t i = 0; i < m_datasets.size(); i++)
        {
            ids[i] = m_datasets[i].id;
        }

        lock.unlock();
        for (auto const& id : ids)
        {
            clear_dataset(id);
        }
    }

    uint8_t x_axis_select = 0;
    uint8_t x_fit_select = 1;
    uint8_t y_fit_select = 1;

    int last_n = 10;

    void export_data_csv();

    [[nodiscard]]
    auto get_max_sample()
    {
        std::scoped_lock lock{_data_mutex};
        return static_cast<double>(current_absolute_sample);
    }

    [[nodiscard]]
    double get_max_rel_time()
    {
        std::scoped_lock lock{_data_mutex};

        if (m_datasets.empty())
        {
            return 0.0;
        }

        return std::ranges::max(
            m_datasets.data() | std::views::transform([](auto const& ds) {
                return ds.relative_times.back();
            }));
    }

    [[nodiscard]]
    double get_max_abs_time()
    {
        std::scoped_lock lock{_data_mutex};

        if (m_datasets.empty())
        {
            return 0.0;
        }

        return std::ranges::max(
            m_datasets.data() | std::views::transform([](auto const& ds) {
                return ds.absolute_times.back();
            }));
    }

    [[nodiscard]]
    constexpr std::mutex& get_serial_mutex() noexcept
    {
        return _serial_mutex;
    }

private:
    void add_to_datasets(sparq_message_t const& message);
    void handle_command(sparq_message_t const& message);

    uint32_t current_absolute_sample{};
    uint64_t first_receive_timestamp{};

    Serial& _sp;
    ConsoleWindow& _console_window;

    std::vector<uint8_t> _serial_buffer;
    std::vector<uint8_t> _message_buffer;

    std::vector<uint64_t> _timestamps;
    std::vector<float> _rel_times;

    std::vector<sparq_marker_t> _markers;

    std::thread _receive_thread;
    std::atomic<bool> _running = true;
    std::mutex _data_mutex, _serial_mutex;

    Datasets m_datasets{};
};
