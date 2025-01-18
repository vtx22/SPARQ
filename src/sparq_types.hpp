#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <array>
#include "imgui.h"

#define SPARQ_MESSAGE_HEADER_LENGTH 4
#define SPARQ_BYTES_PER_VALUE_PAIR 5
#define SPARQ_MAX_MESSAGE_LENGTH (SPARQ_MESSAGE_HEADER_LENGTH + 255 * SPARQ_BYTES_PER_VALUE_PAIR + 2)
#define SPARQ_MIN_MESSAGE_LENGTH (SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_BYTES_PER_VALUE_PAIR + 2)
#define SPARQ_DEFAULT_SIGNATURE 0xFF

enum class sparq_header_control_t : uint8_t
{
    LSB_FIRST = (1 << 7),
    CRC_CS = (1 << 6),
    STRING = (1 << 4),
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
};

struct sparq_message_t
{
    sparq_message_header_t header;
    std::vector<uint8_t> ids;
    std::vector<float> values;
    uint16_t checksum;
    bool valid = false;
    uint64_t timestamp;
    bool is_string = false;
    std::string string_data;

    void from_array(const uint8_t *data)
    {
        header.from_array(data);

        is_string = header.control & (uint8_t)sparq_header_control_t::STRING;

        if (is_string)
        {
            string_data = std::string((char *)&data[SPARQ_MESSAGE_HEADER_LENGTH], header.nval * SPARQ_BYTES_PER_VALUE_PAIR);
        }
        else
        {
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
        }

        uint16_t checksum_start = SPARQ_MESSAGE_HEADER_LENGTH + header.nval * SPARQ_BYTES_PER_VALUE_PAIR;
        checksum = (data[checksum_start] << 8) + data[checksum_start + 1];
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
    int rows = 1;
    int cols = 1;
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