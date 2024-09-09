#pragma once

#include "imgui.h"
#include "implot.h"

#include "DataHandler.hpp"

class DataWindow
{
public:
    DataWindow(DataHandler *data_handler);
    ~DataWindow();

    void update();

    void dataset_entries(const std::vector<sparq_dataset_t> &datasets);

private:
    DataHandler *_data_handler;
};
