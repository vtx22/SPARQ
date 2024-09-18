#pragma once

#include "imgui.h"
#include "implot.h"

#include "imgui-SFML.h"

#include "sparq_types.hpp"
#include "DataHandler.hpp"
#include "AssetHolder.hpp"

class DataWindow
{
public:
    DataWindow(DataHandler *data_handler, AssetHolder *asset_holder);
    ~DataWindow();

    void update();

    void dataset_entries(std::vector<sparq_dataset_t> &datasets);

private:
    void show_view_section();
    void show_datasets_section();

    DataHandler *_data_handler;
    AssetHolder *_asset_holder;

    ImTextureID _delete_icon_id, _hide_icon_id, _show_icon_id;
};
