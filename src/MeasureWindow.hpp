#pragma once

#include "imgui.h"
#include "implot.h"

#include "imgui-SFML.h"

#include "sparq_types.hpp"

#include "AssetHolder.hpp"

class MeasureWindow
{
public:
    MeasureWindow(AssetHolder *asset_holder);
    ~MeasureWindow();

    void update();
    void measure_markers();

private:
    std::vector<sparq_marker_t> _markers;
    AssetHolder *_asset_holder;

    ImTextureID _show_icon_id, _hide_icon_id, _delete_icon_id;
};
