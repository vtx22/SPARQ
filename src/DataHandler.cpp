#include "DataHandler.hpp"

namespace spq::data
{
    void DataHandler::receiver_loop()
    {
        using namespace std::chrono;

        while (m_running)
        {
            std::unique_lock serial_lock{m_serial_mutex};

            if (!m_sp.get_open())
            {
                serial_lock.unlock();
                std::this_thread::sleep_for(milliseconds(SPARQ_RECEIVE_LOOP_DELAY_INTERVAL_MS));
                continue;
            }

            auto const received_message = receive_message();
            serial_lock.unlock();

            if (received_message)
            {
                auto const& message = *received_message;

                switch (message.view.type())
                {
                case message_type::string:
                    m_console_window.add_log(message.view.string()->data());
                    break;
                case message_type::command:
                    handle_command(message);
                    break;
                default:
                {
                    auto const dataset_lock = datasets();
                    auto& datasets = dataset_lock.get();
                    datasets.add_from_message(message);
                    break;
                }
                }

                // If a message was received there is a chance there is more in the buffer, so dont do the sleep for now
                continue;
            }

            // Add sleep only once per fixed interval
            static auto last_sleep_time = steady_clock::now();
            auto const current_time = steady_clock::now();
            if (std::chrono::duration_cast<milliseconds>(current_time - last_sleep_time).count() >= SPARQ_RECEIVE_LOOP_DELAY_INTERVAL_MS)
            {
                std::this_thread::sleep_for(SPARQ_RECEIVE_LOOP_DELAY);
                last_sleep_time = current_time;
            }
        }
    }

    void DataHandler::update_markers()
    {
        for (auto& m : m_markers)
        {
            if (m.ds_id == -1)
            {
                continue;
            }

            auto const dataset_lock = datasets();
            auto& datasets = dataset_lock.get();

            auto const& ds = datasets[m.ds_index];

            if (ds.samples.size() < 2)
            {
                continue;
            }

            std::size_t si = 0;
            for (si = 0; si < ds.samples.size(); si++)
            {
                if (ds.samples[si] > m.x)
                {
                    break;
                }
            }

            if (si == 0)
            {
                m.y = 0;
                continue;
            }

            auto const s_upper = ds.samples[si];
            auto const s_lower = ds.samples[si - 1];
            auto const y_upper = ds.y_values[si];
            auto const y_lower = ds.y_values[si - 1];

            m.y = y_lower + (y_upper - y_lower) / (s_upper - s_lower) * (m.x - s_lower);
        }
    }

    void DataHandler::handle_command(timestamped_message_view const& message)
    {
        // auto const dataset_lock = datasets();
        // auto& datasets = dataset_lock.get();
        // auto const& view = message.view;
        //
        // switch (*view.command())
        // {
        // case sender_command::clear_console:
        //     m_console_window.clear_log();
        //     break;
        // case sender_command::set_dataset_name:
        // {
        //     auto const id = message.command_data[0];
        //     auto const ds = datasets.get(id);
        //     auto const new_name = std::string(
        //         reinterpret_cast<char const*>(&message.command_data[1]),
        //         message.command_data.size() - 1);
        //
        //     if (ds.has_value())
        //     {
        //         auto& ds_ref = ds.value().get();
        //         ds_ref.set_name(new_name);
        //     }
        //     else
        //     {
        //         dataset_t new_ds;
        //         new_ds.id = id;
        //         new_ds.set_name(new_name);
        //         new_ds.color = ImPlot::GetColormapColor(ImPlot::GetColormapSize() / 2 + datasets.size());
        //         datasets.add_dataset(new_ds);
        //     }
        //
        //     break;
        // }
        // case sender_command_t::CLEAR_ALL_DATASETS:
        //     datasets.clear_all();
        //     break;
        // case sender_command_t::DELETE_ALL_DATASETS:
        //     datasets.delete_all();
        //     break;
        // case sender_command_t::CLEAR_SINGLE_DATASET:
        //     datasets.clear(message.command_data[0]);
        //     break;
        // case sender_command_t::DELETE_SINGLE_DATASET:
        //     datasets.delete_dataset(message.command_data[0]);
        //     break;
        // case sender_command_t::SWITCH_PLOT_TYPE:
        //     // TODO: Reenable this later however possible: plot_settings.type = (spq::plotting::plot_type)message.command_data[0];
        //     break;
        // default:
        //     break;
        // }
    }

    [[nodiscard]]
    std::optional<timestamped_message_view> DataHandler::receive_message()
    {
        if (auto const msg = m_decoder.next()) // frames left over from the last read
        {
            return timestamped_message_view{.view = *msg, .timestamp = m_rx_timestamp};
        }

        auto const dst = m_decoder.write_span();
        auto const received = m_sp.read(dst.data(), dst.size());

        if (received == 0)
        {
            return std::nullopt;
        }

        m_rx_timestamp = helper::now_ms();
        m_decoder.commit(received);

        if (auto const msg = m_decoder.next())
        {
            return timestamped_message_view{.view = *msg, .timestamp = m_rx_timestamp};
        }

        return std::nullopt;
    }

    void DataHandler::export_datasets_csv(Datasets const& datasets)
    {
        std::cout << "Exporting data to csv...\n";

        if (datasets.empty())
        {
            std::cerr << "No data to export!\n";
            ImGui::InsertNotification({ImGuiToastType::Error, SPARQ_NOTIFY_DURATION_ERR, "No data to export!"});
            return;
        }

        std::ofstream file("export.csv");

        if (!file.is_open())
        {
            std::cerr << "Failed to create csv file!\n";
            ImGui::InsertNotification({ImGuiToastType::Error, SPARQ_NOTIFY_DURATION_ERR, "Failed to create export.csv!"});
            return;
        }

        file << "sample,relative time [s],timestamp";

        for (auto const& ds : datasets.data())
        {
            file << "," << std::to_string(ds.id);

            if (!ds.name.empty())
            {
                file << " [" << ds.name << "]";
            }
        }

        file << "\n";

        auto const& rel_times = datasets.get_relative_times();
        auto const& timestamps = datasets.get_timestamps();

        for (std::size_t i = 0; i < datasets.get_current_absolute_sample(); i++)
        {
            file << std::to_string(i) << "," << std::to_string(rel_times[i]) << "," << std::to_string(timestamps[i]) << ",";

            std::size_t ds_count = 0;
            for (auto const& ds : datasets.data())
            {
                constexpr auto eps = 1e-9;
                auto const it = std::ranges::lower_bound(ds.samples.begin(), ds.samples.end(), static_cast<double>(i) - eps);

                if (it != ds.samples.end() && std::fabs(*it - static_cast<double>(i)) <= eps)
                {
                    std::size_t idx = std::distance(ds.samples.begin(), it);
                    file << std::to_string(ds.y_values[idx]);
                }

                if (ds_count++ < datasets.size() - 1)
                {
                    file << ",";
                }
            }

            file << "\n";
        }

        file.close();

        std::cout << "Export successful!\n";
        ImGui::InsertNotification({ImGuiToastType::Success, SPARQ_NOTIFY_DURATION_OK, "Data exported to export.csv"});
    }
}
