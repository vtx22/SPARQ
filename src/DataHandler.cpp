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
    if (!_sp->get_open())
    {
        return;
    }

    // Read everything thats available
    int len = _sp->read(_serial_buffer.data(), SPARQ_MAX_MESSAGE_LENGTH * 2);

    if (len <= 0)
    {
        return;
    }

    // if (!_within_message)
    {
        _message_buffer.clear();
    }

    // Append newly received part
    _message_buffer.insert(_message_buffer.end(), _serial_buffer.begin(), _serial_buffer.begin() + len);

    // Wait for rest of message if there aren't enough bytes yet
    if (_message_buffer.size() < SPARQ_MIN_MESSAGE_LENGTH)
    {
        return;
    }

    // Search for header signature
    // TODO: Delete previous part of message buffer before
    bool sig_found = false;
    uint32_t sig_index = 0;
    for (uint32_t i = 0; i < _message_buffer.size(); i++)
    {
        if (_message_buffer[i] == SPARQ_DEFAULT_SIGNATURE)
        {
            if (i > _message_buffer.size() - SPARQ_MIN_MESSAGE_LENGTH)
            {
                // Message is not fully in current buffer, wait for the rest
                _within_message = true;
                return;
            }

            sig_found = true;
            sig_index = i;
            break;
        }
    }

    // No signature found, whole buffer can be deleted
    // TODO FIX
    if (!sig_found)
    {
        return;
    }

    // When we got to here signature was found with atleast MIN_MESSAGE_LENGTH bytes
    sparq_message_t message;
    message.from_array(&_message_buffer[sig_index]);

    std::cout << message.values[0] << std::endl;

    // Message fully parsed
    _within_message = false;
}

sparq_message_t DataHandler::receive_message()
{
    sparq_message_t message;

    if (!_sp->get_open())
    {
        return message;
    }
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