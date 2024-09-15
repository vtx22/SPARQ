#pragma once

#include <cstdint>
#include <array>
#include <iostream>
#include <chrono>
#include <iomanip>

#include "sparq_types.hpp"
#include "serial.hpp"
#include "implot.h"

using namespace std::chrono;

class DataHandler
{
public:
    DataHandler(Serial *sp);
    ~DataHandler();

    void update();
    sparq_message_t receive_message();

    const std::vector<sparq_dataset_t> &get_datasets();
    std::vector<sparq_dataset_t> &get_datasets_editable();

    bool add_dataset(const sparq_dataset_t &dataset);
    bool delete_dataset(uint8_t id);

    static uint8_t xor8_cs(const uint8_t *data, uint32_t length);

    uint8_t x_axis_select = 0;

private:
    Serial *_sp;

    std::vector<uint8_t> _serial_buffer;
    std::vector<uint8_t> _message_buffer;

    std::vector<sparq_dataset_t> _datasets;

    void add_to_datasets(const sparq_message_t &message);
};
