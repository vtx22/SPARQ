#pragma once

#include <cstdint>
#include <array>
#include <iostream>

#include "sparq_types.hpp"
#include "serial.hpp"

class DataHandler
{
public:
    DataHandler(Serial *sp);
    ~DataHandler();

    void update();
    sparq_message_t receive_message();

    static uint8_t xor8_cs(const uint8_t *data, uint32_t length);
    static uint16_t xor16_cs(const uint8_t *data, uint32_t length);

private:
    Serial *_sp;

    std::vector<uint8_t> _serial_buffer;
    std::vector<uint8_t> _message_buffer;
    bool _within_message = false;
};
