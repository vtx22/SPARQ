#include "DataHandler.hpp"

DataHandler::DataHandler(Serial *sp, ConsoleWindow *console_window) : _sp(sp), _console_window(console_window)
{
    _sp->set_timeouts(0xFFFFFFFF, 0, 0, 0, 0);
    _serial_buffer.reserve(SPARQ_MAX_MESSAGE_LENGTH * 2);

    _receive_thread = std::thread(&DataHandler::receiver_loop, this);
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
        sparq_message_t message = receive_message();
        if (message.valid)
        {
            std::lock_guard<std::mutex> lock(_data_mutex);

            if (message.message_type == sparq_message_type_t::STRING)
            {
                _console_window->add_log(message.string_data.c_str());
            }
            else
            {
                add_to_datasets(message);
            }
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

    for (auto &m : _markers)
    {
        if (m.ds_id == -1)
        {
            continue;
        }

        auto &ds = _datasets[m.ds_index];

        if (ds.samples.size() < 2)
        {
            continue;
        }

        uint32_t si = 0;
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

        uint32_t s_upper = ds.samples[si];
        uint32_t s_lower = ds.samples[si - 1];
        double y_upper = ds.y_values[si];
        double y_lower = ds.y_values[si - 1];

        m.y = y_lower + (y_upper - y_lower) / (double)(s_upper - s_lower) * (m.x - s_lower);
    }
}

void DataHandler::add_to_datasets(const sparq_message_t &message)
{
    if (first_receive_timestamp == 0)
    {
        first_receive_timestamp = message.timestamp;
    }

    _timestamps.push_back(message.timestamp);
    _rel_times.push_back((message.timestamp - first_receive_timestamp) / 1000.0);

    if (!_console_window->TextOnly)
    {
        _console_window->add_data_to_log(message.ids.data(), message.values.data(), message.nval);
    }

    for (uint16_t i = 0; i < message.nval; i++)
    {
        sparq_dataset_t *ds = nullptr;
        for (uint8_t ds_index = 0; ds_index < _datasets.size(); ds_index++)
        {
            if (_datasets[ds_index].id == message.ids[i])
            {
                ds = &_datasets[ds_index];
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

            double rel_time = (message.timestamp - first_receive_timestamp) / 1000.0;
            double abs_time = message.timestamp / 1000.0;

            ds_new.append_raw_values(current_absolute_sample, rel_time, abs_time, message.values[i]);

            _datasets.push_back(ds_new);

            continue;
        }

        // Dataset already exists but might be empty
        double new_sample = (ds->samples.size() == 0) ? current_absolute_sample : (ds->samples.back() + 1);
        double new_rel_time = (message.timestamp - first_receive_timestamp) / 1000.0;
        double new_abs_time = message.timestamp / 1000.0;
        double new_y_value = message.values[i];

        ds->append_raw_values(new_sample, new_rel_time, new_abs_time, new_y_value);
    }

    current_absolute_sample++;
}

bool DataHandler::add_dataset(const sparq_dataset_t &dataset)
{
    for (const auto &ds : _datasets)
    {
        if (dataset.id == ds.id)
        {
            return false;
        }
    }

    _datasets.push_back(dataset);
    return true;
}

bool DataHandler::delete_dataset(uint8_t id)
{
    for (uint8_t i = 0; i < _datasets.size(); i++)
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
    std::vector<uint8_t> ids(_datasets.size());

    for (uint8_t i = 0; i < _datasets.size(); i++)
    {
        ids[i] = _datasets[i].id;
    }

    for (uint8_t i = 0; i < ids.size(); i++)
    {
        delete_dataset(ids[i]);
    }
}

bool DataHandler::clear_dataset(uint8_t id)
{
    bool ds_found = false;
    for (auto &ds : _datasets)
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
    for (auto &ds : _datasets)
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
    for (const auto &ds : _datasets)
    {
        clear_dataset(ds.id);
    }
}

sparq_message_t DataHandler::receive_message()
{
    sparq_message_t message;

    if (!_sp->get_open())
    {
        return message;
    }

    static bool in_message = false;

    // Read everything thats available
    int len = _sp->read(_serial_buffer.data(), SPARQ_MAX_MESSAGE_LENGTH * 2);
    // Append to message buffer
    _message_buffer.insert(_message_buffer.end(), _serial_buffer.begin(), _serial_buffer.begin() + len);

    if (!in_message)
    {
        // We are waiting for a new message, so check everything that we have for a signature
        for (uint32_t i = 0; i < _message_buffer.size(); i++)
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

    if (message.header.checksum != DataHandler::xor8_cs(_message_buffer.data(), SPARQ_MESSAGE_HEADER_LENGTH - 1))
    {
        // Header checksum is wrong, clear the message buffer from that part
        _message_buffer.erase(_message_buffer.begin(), _message_buffer.begin() + SPARQ_MESSAGE_HEADER_LENGTH);
        in_message = false;
        return message;
    }

    uint32_t total_message_length = SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_CHECKSUM_LENGTH + message.header.payload_length;

    if (_message_buffer.size() < total_message_length)
    {
        // Message is not complete yet
        return message;
    }

    // Finally we got a full message
    message.from_array(_message_buffer.data());

    in_message = false;

    // Check message checksum if enabled, otherwise assume message valid
    if (message.header.control & (uint8_t)sparq_header_control_t::CS_EN)
    {
        message.valid = (message.checksum == DataHandler::xor8_cs(_message_buffer.data(), total_message_length - 1));
    }
    else
    {
        message.valid = true;
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

const std::vector<sparq_dataset_t> &DataHandler::get_datasets()
{
    return _datasets;
}

std::vector<sparq_dataset_t> &DataHandler::get_datasets_editable()
{
    return _datasets;
}

std::vector<sparq_marker_t> &DataHandler::get_markers()
{
    return _markers;
}

void DataHandler::export_data_csv()
{

    std::cout << "Exporting data to csv...\n";

    if (_datasets.size() == 0)
    {
        std::cerr << "No data to export!\n";
        ImGui::InsertNotification({ImGuiToastType::Error, 5000, "No data to export!"});
        return;
    }

    std::ofstream file("export.csv");

    if (!file.is_open())
    {
        std::cerr << "Failed to create csv file!\n";
        ImGui::InsertNotification({ImGuiToastType::Error, 5000, "Failed to create export.csv!"});
        return;
    }

    file << "sample,relative time [s],timestamp";

    for (const auto &ds : _datasets)
    {
        file << "," << std::to_string(ds.id);
    }

    file << "\n";

    for (uint32_t i = 0; i < current_absolute_sample; i++)
    {
        file << std::to_string(i) << "," << std::to_string(_rel_times[i]) << "," << std::to_string(_timestamps[i]) << ",";

        uint8_t ds_count = 0;
        for (const auto &ds : _datasets)
        {
            for (const auto &s : ds.samples)
            {
                if (s == i)
                {
                    file << ds.y_values[i];
                    break;
                }
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
    ImGui::InsertNotification({ImGuiToastType::Success, 5000, "Data exported to export.csv"});
}

uint8_t DataHandler::xor8_cs(const uint8_t *data, uint32_t length)
{
    uint8_t cs = 0x00;

    for (uint32_t i = 0; i < length; i++)
    {
        cs ^= data[i];
    }

    return cs;
}

double DataHandler::get_max_sample() const
{
    return current_absolute_sample;
}

double DataHandler::get_max_rel_time() const
{
    double max_rel_time = 0;

    for (const auto &ds : _datasets)
    {
        if (ds.relative_times.back() > max_rel_time)
        {
            max_rel_time = ds.relative_times.back();
        }
    }

    return max_rel_time;
}

double DataHandler::get_max_abs_time() const
{
    double max_abs_time = 0;

    for (const auto &ds : _datasets)
    {
        if (ds.relative_times.back() > max_abs_time)
        {
            max_abs_time = ds.absolute_times.back();
        }
    }

    return max_abs_time;
}