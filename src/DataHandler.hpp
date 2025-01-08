#pragma once

#include <cstdint>
#include <array>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>

#include "sparq_types.hpp"
#include "serial.hpp"
#include "implot.h"

#include "ImGuiNotify.hpp"

#include "ConsoleWindow.hpp"

using namespace std::chrono;

class DataHandler
{
public:
    DataHandler(Serial *sp, ConsoleWindow *console_window);
    ~DataHandler();

    void update();
    sparq_message_t receive_message();

    const std::vector<sparq_dataset_t> &get_datasets();
    std::vector<sparq_dataset_t> &get_datasets_editable();

    std::vector<sparq_marker_t> &get_markers();
    void update_markers();

    bool add_dataset(const sparq_dataset_t &dataset);
    bool delete_dataset(uint8_t id);
    void delete_all_datasets();

    bool clear_dataset(uint8_t id);
    void clear_all_datasets();

    static uint8_t xor8_cs(const uint8_t *data, uint32_t length);

    uint8_t x_axis_select = 0;
    uint8_t x_fit_select = 1;
    uint8_t y_fit_select = 1;

    int last_n = 10;

    void export_data_csv();

    double get_max_sample() const;
    double get_max_rel_time() const;
    double get_max_abs_time() const;

    sparq_plot_settings_t plot_settings;

private:
    uint32_t current_absolute_sample = 0;
    uint64_t first_receive_timestamp = 0;

    Serial *_sp;
    ConsoleWindow *_console_window;

    std::vector<uint8_t> _serial_buffer;
    std::vector<uint8_t> _message_buffer;

    std::vector<sparq_dataset_t> _datasets;
    std::vector<uint64_t> _timestamps;
    std::vector<float> _rel_times;

    std::vector<sparq_marker_t> _markers;

    void add_to_datasets(const sparq_message_t &message);
};
