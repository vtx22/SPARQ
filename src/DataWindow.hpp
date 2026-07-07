#pragma once

#include "Window.hpp"
#include "imgui-SFML.h"
#include "implot.h"
#include "sparq_types.hpp"

class DataWindow final : public Window
{
public:
    explicit DataWindow(DataHandler& data_handler)
        : Window(ICON_FA_DATABASE "  Data", data_handler)
    {
    }

private:
    void show_dataset_entries(Datasets& datasets) const;
    void show_datasets_section(Datasets& datasets);

protected:
    void update_content(Datasets& datasets) override;
};
