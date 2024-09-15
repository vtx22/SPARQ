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
                ds.samples.push_back(ds.samples.back() + 1);
                ds.relative_times.push_back((message.timestamp - first_receive_timestamp) / 1000.0);
                ds.absolute_times.push_back(message.timestamp / 1000.0);

                ds.y_values.push_back(message.values[i]);

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

        sparq_dataset ds;
        ds.id = message.ids[i];
        ds.color = ImPlot::GetColormapColor(ds.id);

        ds.samples.push_back(current_absolute_sample);
        ds.relative_times.push_back((message.timestamp - first_receive_timestamp) / 1000.0);
        ds.absolute_times.push_back(message.timestamp / 1000.0);

        ds.y_values.push_back(message.values[i]);

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
            }

            return true;
        }
    }

    return false;
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