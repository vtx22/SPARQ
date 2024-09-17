#pragma once

#include <cstdint>
#include <array>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <tuple>

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
    void delete_all_datasets();

    static uint8_t xor8_cs(const uint8_t *data, uint32_t length);

    uint8_t x_axis_select = 0;
    uint8_t x_fit_select = 0;

    bool interpolation = false;
    int ip_values_per_step = 20;

    static std::tuple<std::vector<double>, std::vector<double>> interpolate(double x0, double y0, double x1, double y1, int steps);
    static std::vector<double> interpolate_x(double x0, double x1, int steps);
    static std::vector<double> interpolate_y(double y0, double y1, int steps);

    static std::vector<double> &add_value_interpolated_x(std::vector<double> &data, double new_value, int steps);
    static std::vector<double> &add_value_interpolated_y(std::vector<double> &data, double new_value, int steps);

    double get_max_sample();
    double get_max_rel_time();
    double get_max_abs_time();

private:
    uint32_t current_absolute_sample = 0;
    uint64_t first_receive_timestamp = 0;

    Serial *_sp;

    std::vector<uint8_t> _serial_buffer;
    std::vector<uint8_t> _message_buffer;

    std::vector<sparq_dataset_t> _datasets;

    void add_to_datasets(const sparq_message_t &message);
};
