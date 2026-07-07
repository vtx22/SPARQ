#include "DataHandler.hpp"

void DataHandler::receiver_loop()
{
    using namespace std::chrono;

    while (m_running)
    {
        std::unique_lock serial_lock(m_serial_mutex);

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
            auto const& message = received_message.value();
            std::scoped_lock lock{m_data_mutex};

            switch (message.message_type)
            {
            case sparq_message_type_t::STRING:
                m_console_window.add_log(message.string_data.c_str());
                break;
            case sparq_message_type_t::SENDER_COMMAND:
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

void DataHandler::handle_command(sparq_message_t const& message)
{
    auto const dataset_lock = datasets();
    auto& datasets = dataset_lock.get();

    switch (message.command_type)
    {
    case sparq_sender_command_t::CLEAR_CONSOLE:
        m_console_window.clear_log();
        break;
    case sparq_sender_command_t::SET_DATASET_NAME:
    {
        auto const id = message.command_data[0];
        auto const ds = datasets.get(id);
        auto const new_name = std::string(
            reinterpret_cast<char const*>(&message.command_data[1]),
            message.command_data.size() - 1);

        if (ds.has_value())
        {
            auto& ds_ref = ds.value().get();
            ds_ref.name = new_name;
            std::strncpy(ds_ref.name_buffer, new_name.c_str(), sizeof(ds_ref.name_buffer));
        }
        else
        {
            sparq_dataset_t new_ds;
            new_ds.id = id;
            new_ds.name = new_name;
            new_ds.color = ImPlot::GetColormapColor(ImPlot::GetColormapSize() / 2 + datasets.size());
            std::strncpy(new_ds.name_buffer, new_name.c_str(), sizeof(new_ds.name_buffer));

            datasets.add_dataset(new_ds);
        }

        break;
    }
    case sparq_sender_command_t::CLEAR_ALL_DATASETS:
        datasets.clear_all();
        break;
    case sparq_sender_command_t::DELETE_ALL_DATASETS:
        datasets.delete_all();
        break;
    case sparq_sender_command_t::CLEAR_SINGLE_DATASET:
        datasets.clear(message.command_data[0]);
        break;
    case sparq_sender_command_t::DELETE_SINGLE_DATASET:
        datasets.delete_dataset(message.command_data[0]);
        break;
    case sparq_sender_command_t::SWITCH_PLOT_TYPE:
        // TODO: Reenable this later however possible: plot_settings.type = (spq::plotting::plot_type)message.command_data[0];
        break;
    default:
        break;
    }
}

std::optional<sparq_message_t> DataHandler::receive_message()
{
    static bool in_message = false;

    // Read everything that's available
    auto const len = m_sp.read(m_serial_buffer.data(), SPARQ_MAX_MESSAGE_LENGTH * 2);

    if (len <= 0 && m_message_buffer.empty())
    {
        return std::nullopt;
    }

    // Append to message buffer
    m_message_buffer.insert(m_message_buffer.end(), m_serial_buffer.begin(), m_serial_buffer.begin() + len);

    if (!in_message)
    {
        // We are waiting for a new message, so check everything that we have for a signature
        for (size_t i = 0; i < m_message_buffer.size(); i++)
        {
            // TODO: Replace with set signature
            if (m_message_buffer[i] == SPARQ_DEFAULT_SIGNATURE)
            {
                // Delete everything in font of the signature so that the current message is always at the front
                m_message_buffer.erase(m_message_buffer.begin(), m_message_buffer.begin() + i);

                in_message = true;
                break;
            }
        }

        // No signature found, ditch buffer
        if (!in_message)
        {
            m_message_buffer.clear();
            return std::nullopt;
        }
    }

    // If we got here signature was detected and it is at the start of the buffer

    // Message is not complete yet, header is incomplete
    if (m_message_buffer.size() < SPARQ_MESSAGE_HEADER_LENGTH)
    {
        return std::nullopt;
    }

    sparq_message_t message{};
    message.header.from_array(m_message_buffer.data());

    if (message.header.checksum != spq::helper::xor8_cs(m_message_buffer, SPARQ_MESSAGE_HEADER_LENGTH - 1))
    {
        // Header checksum is wrong, clear the message buffer from that part
        m_message_buffer.erase(m_message_buffer.begin(), m_message_buffer.begin() + SPARQ_MESSAGE_HEADER_LENGTH);
        in_message = false;
        return std::nullopt;
    }

    auto const total_message_length = SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_CHECKSUM_LENGTH + message.header.payload_length;

    if (m_message_buffer.size() < total_message_length)
    {
        // Message is not complete yet
        return std::nullopt;
    }

    // Finally we got a full message
    message.from_array(m_message_buffer.data());

    in_message = false;

    // Check message checksum if enabled, otherwise assume message valid
    message.valid = true;
    if (message.header.control & static_cast<uint8_t>(sparq_header_control_t::CS_EN))
    {
        message.valid = (message.checksum == spq::helper::xor8_cs(m_message_buffer, total_message_length - 1));
    }

    if (!message.valid)
    {
        std::cerr << "Message Checksum is wrong!\n";
    }

    // Save current timestep
    using namespace std::chrono;
    message.timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    // Clear the message from the buffer
    m_message_buffer.erase(m_message_buffer.begin(), m_message_buffer.begin() + total_message_length);
    return message;
}

void DataHandler::export_data_csv(Datasets const& datasets)
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

