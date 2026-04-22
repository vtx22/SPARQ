#include "DataHandler.hpp"

DataHandler::DataHandler(Serial& sp, ConsoleWindow& console_window)
    : _sp(sp),
      _console_window(console_window)
{
    _sp.set_timeouts(0xFFFF'FFFF, 0, 0, 0, 0);
    _serial_buffer.reserve(static_cast<std::size_t>(SPARQ_MAX_MESSAGE_LENGTH) * 2);

    _receive_thread = std::thread(&DataHandler::receiver_loop, this);
    std::cout << "Starting receiver thread ...\n";
}

DataHandler::~DataHandler()
{
    _running = false;
    if (_receive_thread.joinable())
    {
        _receive_thread.join();
    }
}

void DataHandler::receiver_loop()
{
    while (_running)
    {
        std::unique_lock<std::mutex> serial_lock(_serial_mutex);

        if (!_sp.get_open())
        {
            serial_lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(SPARQ_RECEIVE_LOOP_DELAY_INTERVAL_MS));
            continue;
        }

        auto const message = receive_message();
        serial_lock.unlock();

        if (message.valid)
        {
            std::lock_guard<std::mutex> data_lock(_data_mutex);

            switch (message.message_type)
            {
            case sparq_message_type_t::STRING:
                _console_window.add_log(message.string_data.c_str());
                break;
            case sparq_message_type_t::SENDER_COMMAND:
                handle_command(message);
                break;
            default:
                add_to_datasets(message);
                break;
            }
        }

        // Add sleep only once per fixed interval
        static auto last_sleep_time = std::chrono::steady_clock::now();
        auto const current_time = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_sleep_time).count() >= SPARQ_RECEIVE_LOOP_DELAY_INTERVAL_MS)
        {
            std::this_thread::sleep_for(SPARQ_RECEIVE_LOOP_DELAY);
            last_sleep_time = current_time;
        }
    }
}

void DataHandler::update()
{
    update_markers();
}

void DataHandler::update_markers()
{
    std::lock_guard<std::mutex> lock(_data_mutex);

    for (auto& m : _markers)
    {
        if (m.ds_id == -1)
        {
            continue;
        }

        auto const& ds = _datasets[m.ds_index];

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

void DataHandler::add_to_datasets(sparq_message_t const& message)
{
    if (first_receive_timestamp == 0)
    {
        first_receive_timestamp = message.timestamp;
    }

    _timestamps.push_back(message.timestamp);
    _rel_times.push_back((message.timestamp - first_receive_timestamp) / 1000.0);

    if (!_console_window.TextOnly)
    {
        _console_window.add_data_to_log(message.ids.data(), message.values.data(), message.nval);
    }

    for (uint16_t i = 0; i < message.nval; i++)
    {
        sparq_dataset_t* ds = nullptr;

        for (auto& d : _datasets)
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
            std::cout << "DS not found, creating new one! ID: " << (int)message.ids[i] << "\n";

            sparq_dataset_t ds_new;

            ds_new.id = message.ids[i];
            ds_new.color = ImPlot::GetColormapColor(ImPlot::GetColormapSize() / 2 + _datasets.size());

            auto const rel_time = (message.timestamp - first_receive_timestamp) / 1000.0;
            auto const abs_time = message.timestamp / 1000.0;

            ds_new.append_raw_values(current_absolute_sample, rel_time, abs_time, message.values[i]);
            _datasets.push_back(ds_new);

            continue;
        }

        // Dataset already exists but might be empty
        auto const new_sample = (ds->samples.size() == 0) ? static_cast<double>(current_absolute_sample)
                                                          : (ds->samples.back() + 1.0);
        auto const new_rel_time = (message.timestamp - first_receive_timestamp) / 1000.0;
        auto const new_abs_time = message.timestamp / 1000.0;
        auto const new_y_value = message.values[i];

        ds->append_raw_values(new_sample, new_rel_time, new_abs_time, new_y_value);
    }

    current_absolute_sample++;
}

void DataHandler::handle_command(sparq_message_t const& message)
{
    switch (message.command_type)
    {
    case sparq_sender_command_t::CLEAR_CONSOLE:
        _console_window.clear_log();
        break;
    case sparq_sender_command_t::SET_DATASET_NAME:
    {
        auto const id = message.command_data[0];
        auto const ds = get_dataset(id);
        auto const new_name = std::string(
            reinterpret_cast<const char*>(&message.command_data[1]),
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
            new_ds.color = ImPlot::GetColormapColor(ImPlot::GetColormapSize() / 2 + _datasets.size());
            std::strncpy(new_ds.name_buffer, new_name.c_str(), sizeof(new_ds.name_buffer));

            add_dataset(new_ds);
        }

        break;
    }
    case sparq_sender_command_t::CLEAR_ALL_DATASETS:
        clear_all_datasets();
        break;
    case sparq_sender_command_t::DELETE_ALL_DATASETS:
        delete_all_datasets();
        break;
    case sparq_sender_command_t::CLEAR_SINGLE_DATASET:
        clear_dataset(message.command_data[0]);
        break;
    case sparq_sender_command_t::DELETE_SINGLE_DATASET:
        delete_dataset(message.command_data[0]);
        break;
    case sparq_sender_command_t::SWITCH_PLOT_TYPE:
        // TODO: Reenable this later however possible: plot_settings.type = (spq::plotting::plot_type)message.command_data[0];
        break;
    default:
        break;
    }
}

bool DataHandler::add_dataset(sparq_dataset_t const& dataset)
{
    for (auto const& ds : _datasets)
    {
        if (dataset.id == ds.id)
        {
            return false;
        }
    }

    _datasets.push_back(dataset);
    return true;
}

std::optional<std::reference_wrapper<sparq_dataset_t>> DataHandler::get_dataset(uint8_t const id)
{
    for (auto& ds : _datasets)
    {
        if (ds.id == id)
        {
            return ds;
        }
    }

    return std::nullopt;
}

bool DataHandler::delete_dataset(uint8_t const id)
{
    for (std::size_t i = 0; i < _datasets.size(); i++)
    {
        if (_datasets[i].id == id)
        {
            _datasets.erase(_datasets.begin() + i);

            if (_datasets.size() == 0)
            {
                first_receive_timestamp = 0;
                current_absolute_sample = 0;
            }

            return true;
        }
    }

    return false;
}

void DataHandler::delete_all_datasets()
{
    std::cout << "Deleting all datasets ...\n";
    std::vector<uint8_t> ids(_datasets.size());

    for (std::size_t i = 0; i < _datasets.size(); i++)
    {
        ids[i] = _datasets[i].id;
    }

    for (auto const& id : ids)
    {
        delete_dataset(id);
    }
}

bool DataHandler::clear_dataset(uint8_t const id)
{
    bool ds_found = false;
    for (auto& ds : _datasets)
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
    for (auto const& ds : _datasets)
    {
        if (ds.samples.size() > 0)
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

void DataHandler::clear_all_datasets()
{
    std::cout << "Clearing all datasets ...\n";
    for (auto const& ds : _datasets)
    {
        clear_dataset(ds.id);
    }
}

void DataHandler::hide_all_datasets()
{
    for (auto& ds : _datasets)
    {
        ds.hide = true;
    }
}

void DataHandler::show_all_datasets()
{
    for (auto& ds : _datasets)
    {
        ds.show = true;
    }
}

sparq_message_t DataHandler::receive_message()
{
    sparq_message_t message;
    static bool in_message = false;

    // Read everything thats available
    auto const len = _sp.read(_serial_buffer.data(), SPARQ_MAX_MESSAGE_LENGTH * 2);

    if (len <= 0 && _message_buffer.size() == 0)
    {
        return message;
    }

    // Append to message buffer
    _message_buffer.insert(_message_buffer.end(), _serial_buffer.begin(), _serial_buffer.begin() + len);

    if (!in_message)
    {
        // We are waiting for a new message, so check everything that we have for a signature
        for (size_t i = 0; i < _message_buffer.size(); i++)
        {
            // TODO: Replace with set signature
            if (_message_buffer[i] == SPARQ_DEFAULT_SIGNATURE)
            {
                // Delete everything in font of the signature so that the current message is always at the front
                _message_buffer.erase(_message_buffer.begin(), _message_buffer.begin() + i);

                in_message = true;
                break;
            }
        }

        // No signature found, ditch buffer
        if (!in_message)
        {
            _message_buffer.clear();
            return message;
        }
    }

    // If we got here signature was detected and it is at the start of the buffer

    // Message is not complete yet, header is incomplete
    if (_message_buffer.size() < SPARQ_MESSAGE_HEADER_LENGTH)
    {
        return message;
    }

    message.header.from_array(_message_buffer.data());

    if (message.header.checksum != spq::helper::xor8_cs(_message_buffer.data(), SPARQ_MESSAGE_HEADER_LENGTH - 1))
    {
        // Header checksum is wrong, clear the message buffer from that part
        _message_buffer.erase(_message_buffer.begin(), _message_buffer.begin() + SPARQ_MESSAGE_HEADER_LENGTH);
        in_message = false;
        return message;
    }

    auto const total_message_length = SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_CHECKSUM_LENGTH + message.header.payload_length;

    if (_message_buffer.size() < total_message_length)
    {
        // Message is not complete yet
        return message;
    }

    // Finally we got a full message
    message.from_array(_message_buffer.data());

    in_message = false;

    // Check message checksum if enabled, otherwise assume message valid
    message.valid = true;
    if (message.header.control & static_cast<uint8_t>(sparq_header_control_t::CS_EN))
    {
        message.valid = (message.checksum == spq::helper::xor8_cs(_message_buffer.data(), total_message_length - 1));
    }

    if (!message.valid)
    {
        std::cerr << "Message Checksum is wrong!\n";
    }

    // Save current timestep
    message.timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    // Clear the message from the buffer
    _message_buffer.erase(_message_buffer.begin(), _message_buffer.begin() + total_message_length);
    return message;
}

void DataHandler::export_data_csv() const
{
    std::cout << "Exporting data to csv...\n";
    // std::lock_guard<std::mutex> lock(_data_mutex);

    if (_datasets.size() == 0)
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

    for (auto const& ds : _datasets)
    {
        file << "," << std::to_string(ds.id);

        if (!ds.name.empty())
        {
            file << " [" << ds.name << "]";
        }
    }

    file << "\n";

    for (std::size_t i = 0; i < current_absolute_sample; i++)
    {
        file << std::to_string(i) << "," << std::to_string(_rel_times[i]) << "," << std::to_string(_timestamps[i]) << ",";

        std::size_t ds_count = 0;
        for (auto const& ds : _datasets)
        {
            constexpr auto eps = 1e-9;
            auto const it = std::lower_bound(ds.samples.begin(), ds.samples.end(), static_cast<double>(i) - eps);

            if (it != ds.samples.end() && std::fabs(*it - static_cast<double>(i)) <= eps)
            {
                std::size_t idx = std::distance(ds.samples.begin(), it);
                file << std::to_string(ds.y_values[idx]);
            }

            if (ds_count++ < _datasets.size() - 1)
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

