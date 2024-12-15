#pragma once

#include <array>
#include "imgui.h"
#include "serial.hpp"

#include <string>
#include <vector>
#include <cmath>

#include "AssetHolder.hpp"

#include "ImGuiNotify.hpp"
#include "IconsFontAwesome6.h"

class ConnectionWindow
{

public:
    ConnectionWindow(Serial *sp, AssetHolder *asset_holder);
    ~ConnectionWindow();

    void update();

    std::string get_selected_port();
    int get_selected_index();

private:
    int _current_id = 0;
    std::vector<std::string> _com_ports;

    int _baud_rate = 115200;
    const std::array<int, 6> _available_baud_rates = {9600, 19200, 38400, 57600, 115200, 230400};
    char _signature_chars[3] = {'F', 'F', 0};
    uint8_t _signature = 0xFF;

    bool _port_open = false;
    int _selected_comm_mode = 0;

    uint8_t hex_chars_to_byte(char high, char low);

    Serial *_sp;
    AssetHolder *_asset_holder;

    ImTextureID _refresh_icon;
};
