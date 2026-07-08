#pragma once

#include "../sparq_types.hpp"
#include "Window.hpp"
#include "imgui-SFML.h"
#include "implot.h"

namespace spq::ui
{
    class DataWindow final : public Window
    {
    public:
        explicit DataWindow(data::DataHandler& data_handler)
            : Window(ICON_FA_DATABASE "  Data", data_handler)
        {
        }

    private:
        void show_dataset_entries(data::Datasets& datasets) const;
        void show_datasets_section(data::Datasets& datasets);

    protected:
        void update_content(data::Datasets& datasets) override;
    };
}
