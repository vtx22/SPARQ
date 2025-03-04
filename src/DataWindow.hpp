#pragma once

#include "Window.hpp"

#include "implot.h"
#include "imgui-SFML.h"

#include "sparq_types.hpp"

class DataWindow : public Window
{
public:
    DataWindow(DataHandler *data_handler) : Window(ICON_FA_DATABASE "  Data", data_handler) {}

    void update_content();

    void dataset_entries(std::vector<sparq_dataset_t> &datasets);

private:
    void show_datasets_section();
};
