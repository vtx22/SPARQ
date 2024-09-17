#include "DataHandler.hpp"

DataHandler::DataHandler(Serial *sp) : _sp(sp)
{
    _sp->set_timeouts(0xFFFFFFFF, 0, 0, 0, 0);
    _serial_buffer.reserve(SPARQ_MAX_MESSAGE_LENGTH * 2);
}

DataHandler::~DataHandler()
{
}

void DataHandler::update()
{
    sparq_message_t message = receive_message();
    if (!message.valid)
    {
        return;
    }

    // Received complete message
    std::cout << "Received message! First value: " << message.values[0] << "\n";

    add_to_datasets(message);
}

void DataHandler::add_to_datasets(const sparq_message_t &message)
{
    for (uint8_t i = 0; i < message.header.nval; i++)
    {
        bool ds_found = false;
        for (auto &ds : _datasets)
        {
            if (ds.id == message.ids[i])
            {
                // All new x and y values
                double new_sample = ds.samples.back() + 1;
                double new_rel_time = (message.timestamp - first_receive_timestamp) / 1000.0;
                double new_abs_time = message.timestamp / 1000.0;
                double new_y_value = message.values[i];

                // Add interpolated values from last value to new value
                add_value_interpolated_x(ds.samples_ip, new_sample, ip_values_per_step);
                add_value_interpolated_x(ds.relative_times_ip, new_rel_time, ip_values_per_step);
                add_value_interpolated_x(ds.absolute_times_ip, new_abs_time, ip_values_per_step);
                add_value_interpolated_y(ds.y_values_ip, new_y_value, ip_values_per_step);

                // Add uninterpolated values
                ds.append_raw_values(new_sample, new_rel_time, new_abs_time, new_y_value);

                std::cout << "Adding values: " << (ds.samples.back() + 1) << " " << message.timestamp / 1000.0 << "\n";
                ds_found = true;
                break;
            }
        }

        if (ds_found)
        {
            continue;
        }

        if (first_receive_timestamp == 0)
        {
            first_receive_timestamp = message.timestamp;
        }

        std::cout << "DS not found, creating new one! " << (int)message.ids[i] << "\n";

        sparq_dataset_t ds;
        ds.id = message.ids[i];
        ds.color = ImPlot::GetColormapColor(ImPlot::GetColormapSize() / 2 + _datasets.size());

        double rel_time = (message.timestamp - first_receive_timestamp) / 1000.0;
        double abs_time = message.timestamp / 1000.0;

        ds.append_start_values(current_absolute_sample, rel_time, abs_time, message.values[i]);

        _datasets.push_back(ds);
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
    for (const auto &ds : _datasets)
    {
        delete_dataset(ds.id);
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
        std::cout << "Message Header Checksum is wrong!\n";
        _message_buffer.erase(_message_buffer.begin(), _message_buffer.begin() + SPARQ_MESSAGE_HEADER_LENGTH);
        in_message = false;
        return message;
    }

    uint32_t total_message_length = SPARQ_MESSAGE_HEADER_LENGTH + 2 + message.header.nval * SPARQ_BYTES_PER_VALUE_PAIR;

    if (_message_buffer.size() < total_message_length)
    {
        // Message is not complete yet, we are expecting nval id/value pairs
        return message;
    }

    // std::cout << std::hex << std::setfill('0') << std::setw(2);
    // for (auto b : _message_buffer)
    // {
    //     std::cout << (int)b << " ";
    // }
    // std::cout << "\n";

    // Finally we got a full message
    message.from_array(_message_buffer.data());

    in_message = false;

    message.valid = message.checksum == (uint16_t)DataHandler::xor8_cs(_message_buffer.data(), total_message_length - 2);

    if (!message.valid)
    {
        std::cout << "Message Checksum is wrong!\n";
    }
    std::cout << "TML: " << total_message_length << " NVAL: " << (int)message.header.nval << "\n";
    std::cout << "Values: " << message.values[0] << " " << message.values[1] << "\n\n";

    message.timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
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

uint8_t DataHandler::xor8_cs(const uint8_t *data, uint32_t length)
{
    uint8_t cs = 0x00;

    for (uint32_t i = 0; i < length; i++)
    {
        cs ^= data[i];
    }

    return cs;
}

std::tuple<std::vector<double>, std::vector<double>> DataHandler::interpolate(double x0, double y0, double x1, double y1, int steps)
{
    return {interpolate_x(x0, x1, steps), interpolate_y(y0, y1, steps)};
}

std::vector<double> DataHandler::interpolate_x(double x0, double x1, int steps)
{
    std::vector<double> x_values(steps + 1);
    for (uint8_t i = 0; i <= steps; i++)
    {
        double x = i * (x1 - x0) / steps;
        x_values[i] = x0 + x;
    }

    return x_values;
}

std::vector<double> DataHandler::interpolate_y(double y0, double y1, int steps)
{
    std::vector<double> y_values(steps + 1);

    double d = y0;
    double c = 0;
    double b = 3 * (y1 - d) - 2 * c - 0;
    double a = y1 - b - c - d;

    for (uint8_t i = 0; i <= steps; i++)
    {
        double x = i / (double)steps;
        y_values[i] = a * x * x * x + b * x * x + c * x + d;
    }

    return y_values;
}

std::vector<double> &DataHandler::add_value_interpolated_x(std::vector<double> &data, double new_value, int steps)
{
    if (data.size() == 0)
    {
        return data;
    }

    auto interpolated = interpolate_x(data.back(), new_value, steps);
    data.pop_back();
    data.insert(data.end(), interpolated.begin(), interpolated.end());
    return data;
}

std::vector<double> &DataHandler::add_value_interpolated_y(std::vector<double> &data, double new_value, int steps)
{
    if (data.size() == 0)
    {
        return data;
    }

    auto interpolated = interpolate_y(data.back(), new_value, steps);
    data.pop_back();
    data.insert(data.end(), interpolated.begin(), interpolated.end());
    return data;
}

double DataHandler::get_max_sample()
{
    return current_absolute_sample;
}

double DataHandler::get_max_rel_time()
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

double DataHandler::get_max_abs_time()
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