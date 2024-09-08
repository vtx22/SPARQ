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
    if (!receive_message())
    {
        return;
    }

    // Received complete message
    std::cout << "Received message! First value: " << _last_message.values[0] << "\n";
}

bool DataHandler::receive_message()
{
    if (!_sp->get_open())
    {
        return false;
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
            return false;
        }
    }

    // If we got here signature was detected and it is at the start of the buffer

    // Message is not complete yet, header is incomplete
    if (_message_buffer.size() < SPARQ_MESSAGE_HEADER_LENGTH)
    {
        return false;
    }

    _last_message.header.from_array(_message_buffer.data());

    if (_last_message.header.checksum != DataHandler::xor8_cs(_message_buffer.data(), SPARQ_MESSAGE_HEADER_LENGTH - 1))
    {
        // Header checksum is wrong, clear the message buffer from that part
        std::cout << "Message Header Checksum is wrong!\n";
        _message_buffer.erase(_message_buffer.begin(), _message_buffer.begin() + SPARQ_MESSAGE_HEADER_LENGTH);
        in_message = false;
        return false;
    }

    uint32_t total_message_length = SPARQ_MESSAGE_HEADER_LENGTH + 2 + _last_message.header.nval * SPARQ_BYTES_PER_VALUE_PAIR;

    if (_message_buffer.size() < total_message_length)
    {
        // Message is not complete yet, we are expecting nval id/value pairs
        return false;
    }

    // Finally we got a full message
    _last_message.from_array(_message_buffer.data());

    if (_last_message.checksum != DataHandler::xor16_cs(_message_buffer.data(), total_message_length - 2))
    {
        // Checksum is wrong
        std::cout << "Message Checksum is wrong!\n";
        return false;
    }

    _message_buffer.erase(_message_buffer.begin(), _message_buffer.begin() + total_message_length);
    in_message = false;
    return true;
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

uint16_t DataHandler::xor16_cs(const uint8_t *data, uint32_t length)
{
    uint16_t cs = 0x0000;

    for (uint32_t i = 0; i < length; i++)
    {
        cs ^= data[i];
    }

    return cs;
}