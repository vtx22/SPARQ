#pragma once

#include "imgui.h"

#include "IconsFontAwesome6.h"

#include "DataHandler.hpp"

class Window
{
public:
    Window(const char *name, DataHandler *data_handler) : _name(name), _data_handler(data_handler) {}
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
    DataHandler *_data_handler;
    const char *_name;
};