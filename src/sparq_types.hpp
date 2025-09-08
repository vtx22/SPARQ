#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <array>

#include "imgui.h"

#include "sparq_config.h"

#define SPARQ_MESSAGE_HEADER_LENGTH 5
#define SPARQ_BYTES_PER_VALUE_PAIR 5
#define SPARQ_CHECKSUM_LENGTH 1
#define SPARQ_MAX_PAYLOAD_LENGTH 0xFFFF
#define SPARQ_MIN_MESSAGE_LENGTH (SPARQ_MESSAGE_HEADER_LENGTH + 1 + SPARQ_CHECKSUM_LENGTH)
#define SPARQ_MAX_MESSAGE_LENGTH (SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_MAX_PAYLOAD_LENGTH + SPARQ_CHECKSUM_LENGTH)
#define SPARQ_DEFAULT_SIGNATURE 0xFF

constexpr bool sparq_is_little_endian()
{
    constexpr uint32_t value = 0x01020304;
    return reinterpret_cast<const uint8_t *>(&value)[0] == 0x04;
}

enum class sparq_header_control_t : uint8_t
{
    LSB_FIRST = (1 << 7),
    CS_EN = (1 << 6),
    MSG_TYPE = (1 << 2) + (1 << 3),
    SIGNED = (1 << 1),
    INTEGER = (1 << 0),
};

enum class sparq_message_type_t : uint8_t
{
    ID_PAIR = 0b00,
    STRING = 0b01,
    BULK_SINGLE_ID = 0b10,
    SENDER_COMMAND = 0b11,
};

enum class sparq_sender_command_t : uint8_t
{
    CLEAR_CONSOLE,         // Remote clear the console
    CLEAR_ALL_DATASETS,    // Remote clear all datasets data but keep the settings
    CLEAR_SINGLE_DATASET,  // Remote clear a single datasets data but keep the settings
    DELETE_ALL_DATASETS,   // Remote delete all datasets
    DELETE_SINGLE_DATASET, // Remote delete single dataset
    SET_DATASET_NAME,      // Remote set the name of a given dataset
    SWITCH_PLOT_TYPE,      // Remote switch the plot type (e.g. line, heatmap, etc.)
};

struct sparq_dataset_t
{
    int16_t id = 0;
    char name_buffer[64] = {0};
    std::string name;
    ImVec4 color = ImVec4(1, 0, 0, 1);
    bool toggle_visibility = false;
    bool hidden = false;
    bool display_square = false;

    std::vector<double> absolute_times;
    std::vector<double> relative_times;
    std::vector<double> samples;
    std::vector<double> y_values;

    void append_raw_values(double sample, double rel_time, double abs_time, double y_value)
    {
        samples.push_back(sample);
        relative_times.push_back(rel_time);
        absolute_times.push_back(abs_time);
        y_values.push_back(y_value);
    }

    void clear()
    {
        absolute_times.clear();
        relative_times.clear();
        samples.clear();
        y_values.clear();
    }
};

struct sparq_message_header_t
{
    uint8_t signature;
    uint8_t control;
    uint16_t payload_length;
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
        if ((bool)(control & (uint8_t)sparq_header_control_t::LSB_FIRST) == sparq_is_little_endian())
        {
            payload_length = (buffer[2] << 8) + buffer[3];
        }
        else
        {
            payload_length = (buffer[3] << 8) + buffer[2];
        }

        checksum = buffer[4];
    }

    void to_array(uint8_t *buffer)
    {
        buffer[0] = signature;
        buffer[1] = control;
        buffer[2] = payload_length >> 8;
        buffer[3] = payload_length & 0xFF;
        buffer[4] = checksum;
    }
};

struct sparq_message_t
{
    sparq_message_header_t header;
    std::vector<uint8_t> ids;
    std::vector<double> values;
    uint16_t checksum;
    bool valid = false;
    uint64_t timestamp;
    std::string string_data;
    sparq_message_type_t message_type;
    sparq_sender_command_t command_type;
    std::vector<uint8_t> command_data;
    uint16_t nval = 0;

    double buffer_to_double(const uint8_t *data)
    {
        uint8_t v3 = data[0];
        uint8_t v2 = data[1];
        uint8_t v1 = data[2];
        uint8_t v0 = data[3];

        uint32_t value32 = 0;

        bool lsb_first = header.control & (uint8_t)sparq_header_control_t::LSB_FIRST;
        if (lsb_first == sparq_is_little_endian())
        {
            value32 = (v3 << 24) + (v2 << 16) + (v1 << 8) + v0;
        }
        else
        {
            value32 = (v0 << 24) + (v1 << 16) + (v2 << 8) + v3;
        }

        double value = 0;
        if (header.control & (uint8_t)sparq_header_control_t::INTEGER)
        {
            if (header.control & (uint8_t)sparq_header_control_t::SIGNED)
            {
                value = *(int32_t *)&value32;
            }
            else
            {
                value = *(uint32_t *)&value32;
            }
        }
        else
        {
            value = *(float *)&value32;
        }

        return value;
    }

    void parse_msg_id_pair(const uint8_t *data)
    {
        nval = header.payload_length / SPARQ_BYTES_PER_VALUE_PAIR;
        ids.resize(nval);
        values.resize(nval);

        for (uint8_t pair = 0; pair < nval; pair++)
        {
            uint16_t pair_index = SPARQ_MESSAGE_HEADER_LENGTH + pair * SPARQ_BYTES_PER_VALUE_PAIR;

            ids[pair] = data[pair_index];
            values[pair] = buffer_to_double(&data[pair_index + 1]);
        }
    }

    void parse_msg_bulk_single_id(const uint8_t *data)
    {
        nval = (header.payload_length - 1) / 4;

        ids.assign(nval, data[SPARQ_MESSAGE_HEADER_LENGTH]);
        values.resize(nval);

        const uint8_t *ptr = data + SPARQ_MESSAGE_HEADER_LENGTH + 1;
        for (double &value : values)
        {
            value = buffer_to_double(ptr);
            ptr += 4;
        }
    }

    void parse_msg_sender_command(const uint8_t *data)
    {
        command_type = static_cast<sparq_sender_command_t>(data[SPARQ_MESSAGE_HEADER_LENGTH]);

        if (header.payload_length <= 1)
        {
            return;
        }

        size_t additional_command_payload_length = header.payload_length - 1;
        const uint8_t *first_payload_ptr = &data[SPARQ_MESSAGE_HEADER_LENGTH];

        command_data.resize(additional_command_payload_length);
        std::copy(first_payload_ptr + 1, first_payload_ptr + additional_command_payload_length + 1, command_data.begin());
    }

    void from_array(const uint8_t *data)
    {
        header.from_array(data);

        if (header.payload_length == 0)
        {
            return;
        }

        message_type = (sparq_message_type_t)((header.control >> 2) & 0b11);

        switch (message_type)
        {
        case sparq_message_type_t::STRING:
            string_data = std::string((char *)&data[SPARQ_MESSAGE_HEADER_LENGTH], header.payload_length);
            break;
        case sparq_message_type_t::ID_PAIR:
            parse_msg_id_pair(data);
            break;
        case sparq_message_type_t::BULK_SINGLE_ID:
            parse_msg_bulk_single_id(data);
            break;
        case sparq_message_type_t::SENDER_COMMAND:
            parse_msg_sender_command(data);
            break;
        default:
            break;
        }

        checksum = data[SPARQ_MESSAGE_HEADER_LENGTH + header.payload_length];
    }
};

struct sparq_axis_t
{
    const char *dropdown_name;
    const char *axis_label;
};

struct sparq_marker_t
{
    std::string name = "M";
    double x = 0;
    double y = 0;
    uint8_t ds_index;
    int16_t ds_id = -1;
    bool hidden = false;
    ImVec4 color = ImVec4(1, 1, 1, 1);
};

struct sparq_heatmap_settings_t
{
    bool normalize_xy = false;
    bool equal = false;
    bool show_values = false;
    float scale_min = 0;
    float scale_max = 100;
    bool autoscale = false;
    bool invert_scale = false;
    int rows = 1;
    int cols = 1;
    bool smooth = false;
    int smoothing_factor = 5;
};

enum class sparq_plot_t
{
    LINE,
    HEATMAP,
};

struct sparq_plot_settings_t
{
    sparq_plot_t type = sparq_plot_t::LINE;
    sparq_heatmap_settings_t heatmap_settings;
};

const sparq_axis_t X_AX_SAMPLES = {.dropdown_name = "Samples", .axis_label = "Samples"};
const sparq_axis_t X_AX_REL_TIME = {.dropdown_name = "Relative Time", .axis_label = "Time [s]"};
const sparq_axis_t X_AX_ABS_TIME = {.dropdown_name = "Date Time", .axis_label = "Date"};

const std::array<sparq_axis_t, 3> x_axis_types = {X_AX_SAMPLES, X_AX_REL_TIME, X_AX_ABS_TIME};
const std::array<const char *, 3> x_axis_fits = {"Manual", "Fit All", "Last N"};
const std::array<const char *, 2> y_axis_fits = {"Manual", "Fit All"};