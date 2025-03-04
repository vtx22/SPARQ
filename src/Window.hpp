#pragma once

#include "imgui.h"

#include "IconsFontAwesome6.h"

#include "ConfigHandler.hpp"
#include "DataHandler.hpp"

class Window
{
public:
    Window(const char *name, DataHandler *data_handler) : _name(name), _data_handler(data_handler), _config_handler(ConfigHandler::get_instance()) {}
    virtual ~Window() = default;

    void draw()
    {
        if (ImGui::Begin(_name))
        {
            update_content();
        }

        ImGui::End();
    }

    virtual void update_content() = 0;

protected:
    const char *_name;
    DataHandler *_data_handler;
    ConfigHandler &_config_handler;
};