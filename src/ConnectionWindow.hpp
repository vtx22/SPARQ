#pragma once

#include "Window.hpp"

#include <array>
#include <string>
#include <vector>
#include <cmath>

#include "serial.hpp"

#include "ImGuiNotify.hpp"

class ConnectionWindow : public Window
{
public:
    ConnectionWindow(DataHandler *data_handler, Serial *sp);

    void update_content();

    std::string get_selected_port();
    int get_selected_index();

    size_t update_com_ports_dropdown();

private:
    int _current_id = 0;
    std::vector<std::string> _com_ports;

    int _baud_rate = 115200;
    const std::array<int, 9> _available_baud_rates = {4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
    char _signature_chars[3] = {'F', 'F', 0};
    uint8_t _signature = 0xFF;

    bool _port_open = false;
    int _selected_comm_mode = 0;

    uint8_t hex_chars_to_byte(char high, char low);

    Serial *_sp;
};
