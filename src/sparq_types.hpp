#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <array>
#include "imgui.h"

#define SPARQ_MESSAGE_HEADER_LENGTH 4
#define SPARQ_BYTES_PER_VALUE_PAIR 5
#define SPARQ_MAX_MESSAGE_LENGTH (SPARQ_MESSAGE_HEADER_LENGTH + 255 * SPARQ_BYTES_PER_VALUE_PAIR)
#define SPARQ_MIN_MESSAGE_LENGTH 11
#define SPARQ_DEFAULT_SIGNATURE 0xFF

struct sparq_dataset_t
{
    uint8_t id = 0;
    char name[64] = {0};
    ImVec4 color = ImVec4(1, 0, 0, 1);
    bool toggle_visibility = false;
    bool hidden = false;

    std::vector<double> absolute_times;
    std::vector<double> relative_times;
    std::vector<double> samples;
    std::vector<double> y_values;

    std::vector<double> absolute_times_ip;
    std::vector<double> relative_times_ip;
    std::vector<double> samples_ip;
    std::vector<double> y_values_ip;

    void append_start_values(double first_sample, double rel_time, double abs_time, double first_y)
    {
        samples.push_back(first_sample);
        samples_ip.push_back(first_sample);

        relative_times.push_back(rel_time);
        relative_times_ip.push_back(rel_time);

        absolute_times.push_back(abs_time);
        absolute_times_ip.push_back(abs_time);

        y_values.push_back(first_y);
        y_values_ip.push_back(first_y);
    }

    void append_raw_values(double sample, double rel_time, double abs_time, double y_value)
    {
        samples.push_back(sample);
        relative_times.push_back(rel_time);
        absolute_times.push_back(abs_time);
        y_values.push_back(y_value);
    }

} typedef sparq_dataset_t;

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

        for (uint8_t pair = 0; pair < header.nval; pair++)
        {
            uint16_t pair_index = SPARQ_MESSAGE_HEADER_LENGTH + pair * SPARQ_BYTES_PER_VALUE_PAIR;

            ids[pair] = data[pair_index];
            uint8_t v3 = data[pair_index + 1];
            uint8_t v2 = data[pair_index + 2];
            uint8_t v1 = data[pair_index + 3];
            uint8_t v0 = data[pair_index + 4];

            uint32_t value = (v3 << 24) + (v2 << 16) + (v1 << 8) + v0;
            values.push_back(*(float *)&value);
        }

        uint16_t checksum_start = SPARQ_MESSAGE_HEADER_LENGTH + header.nval * SPARQ_BYTES_PER_VALUE_PAIR;
        checksum = (data[checksum_start] << 8) + data[checksum_start + 1];
    }

} typedef sparq_message_t;

struct sparq_axis_t
{
    const char *dropdown_name;
    const char *axis_label;
} typedef sparq_axis_t;

const sparq_axis_t X_AX_SAMPLES = {.dropdown_name = "Samples", .axis_label = "Samples"};
const sparq_axis_t X_AX_REL_TIME = {.dropdown_name = "Relative Time", .axis_label = "Time [s]"};
const sparq_axis_t X_AX_ABS_TIME = {.dropdown_name = "Date Time", .axis_label = "Date"};

const std::array<sparq_axis_t, 3> x_axis_types = {X_AX_SAMPLES, X_AX_REL_TIME, X_AX_ABS_TIME};
const std::array<const char *, 2> x_axis_fits = {"Fit All", "Last N"};