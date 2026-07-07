#pragma once

#include "Window.hpp"
#include "imgui-SFML.h"
#include "implot.h"
#include "sparq_types.hpp"

class DataWindow final : public Window
{
public:
    DataWindow(DataHandler& data_handler)
        : Window(ICON_FA_DATABASE "  Data", data_handler)
    {
    }

    void dataset_entries(Datasets& datasets) const;

private:
    void show_datasets_section(Datasets& datasets);

protected:
    void update_content(Datasets& datasets) override;
};
