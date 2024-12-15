#pragma once

#include <algorithm>

#include "imgui.h"
#include "implot.h"

#include "imgui-SFML.h"

#include "sparq_types.hpp"

#include "AssetHolder.hpp"
#include "DataHandler.hpp"

#include "IconsFontAwesome6.h"

class MeasureWindow
{
public:
    MeasureWindow(AssetHolder *asset_holder, DataHandler *data_handler);
    ~MeasureWindow();

    void update();
    void measure_markers_table(std::vector<sparq_marker_t> &markers);

private:
    AssetHolder *_asset_holder;
    DataHandler *_data_handler;

    ImTextureID _show_icon_id, _hide_icon_id, _delete_icon_id;
};
