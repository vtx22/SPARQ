#pragma once

#include "imgui.h"
#include "implot.h"
#include <SFML/Graphics.hpp>
#include "SFML/OpenGL.hpp"
#include "imgui-SFML.h"
#include <cstring>

#include "DataHandler.hpp"

class DataWindow
{
public:
    DataWindow(DataHandler *data_handler);
    ~DataWindow();

    void update();

    void dataset_entries(std::vector<sparq_dataset_t> &datasets);
    ImTextureID gl_handle_to_imgui_id(GLuint gl_texture_handle);

private:
    DataHandler *_data_handler;

    sf::Texture _delete_icon, _hide_icon, _show_icon;
    ImTextureID _hide_icon_id, _show_icon_id;
};
