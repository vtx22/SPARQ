#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include "imgui.h"

#define SPARQ_MESSAGE_HEADER_LENGTH 4
#define SPARQ_BYTES_PER_VALUE_PAIR 5
#define SPARQ_MAX_MESSAGE_LENGTH (SPARQ_MESSAGE_HEADER_LENGTH + 255 * SPARQ_BYTES_PER_VALUE_PAIR)
#define SPARQ_MIN_MESSAGE_LENGTH 11
#define SPARQ_DEFAULT_SIGNATURE 0xFF

struct sparq_data_point
{
    float x;
    float y;

} typedef sparq_data_point;

struct sparq_dataset_t
{
    uint8_t id = 0;
    char name[64] = {0};
    ImVec4 color;
    std::vector<float> x_values;
    std::vector<float> y_values;

} typedef sparq_dataset;

struct sparq_message_header_t
{
    uint8_t signature;
    uint8_t control;
    uint8_t nval;
    uint8_t checksum;

    sparq_message_header_t()
    {
    }

    sparq_message_header_t(const uint8_t *buffer)
    {
        from_array(buffer);
    }

    void from_array(const uint8_t *buffer)
    {
        signature = buffer[0];
        control = buffer[1];
        nval = buffer[2];
        checksum = buffer[3];
    }

    void to_array(uint8_t *buffer)
    {
        buffer[0] = signature;
        buffer[1] = control;
        buffer[2] = nval;
        buffer[3] = checksum;
    }

} typedef sparq_message_header_t;

struct sparq_message_t
{
    sparq_message_header_t header;
    std::vector<uint8_t> ids;
    std::vector<float> values;
    uint16_t checksum;
    bool valid = false;
    uint64_t timestamp;

    void from_array(const uint8_t *data)
    {
        header.from_array(data);

        ids.reserve(header.nval);
        values.reserve(header.nval);

        uint8_t start = SPARQ_MESSAGE_HEADER_LENGTH;

        for (uint8_t pair = 0; pair < header.nval; pair++)
        {
            uint16_t pair_index = start + pair;

            ids[pair] = data[pair_index];
            uint8_t v3 = data[pair_index + 1];
            uint8_t v2 = data[pair_index + 2];
            uint8_t v1 = data[pair_index + 3];
            uint8_t v0 = data[pair_index + 4];

            uint32_t value = (v3 << 24) + (v2 << 16) + (v1 << 8) + v0;
            values.push_back(*(float *)&value);
        }

        checksum = (data[SPARQ_MESSAGE_HEADER_LENGTH + header.nval * SPARQ_BYTES_PER_VALUE_PAIR] << 8) + data[SPARQ_MESSAGE_HEADER_LENGTH + header.nval * SPARQ_BYTES_PER_VALUE_PAIR + 1];
    }

} typedef sparq_message_t;
