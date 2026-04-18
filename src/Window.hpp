#pragma once

#include "ConfigHandler.hpp"
#include "DataHandler.hpp"
#include "IconsFontAwesome6.h"
#include "imgui.h"

class Window
{
public:
    Window(std::string name, DataHandler& data_handler)
        : _name(name),
          _data_handler(data_handler),
          _config_handler(ConfigHandler::get_instance())
    {
    }

    virtual ~Window() = default;

    void draw()
    {
        before_imgui_begin();

        if (ImGui::Begin(_name.c_str()))
        {
            update_content();
        }

        ImGui::End();

        after_imgui_end();
    }

    virtual void update_content() = 0;

    virtual void before_imgui_begin()
    {
    }

    virtual void after_imgui_end()
    {
    }

protected:
    std::string _name;
    DataHandler& _data_handler;
    ConfigHandler& _config_handler;
};
