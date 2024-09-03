#ifndef CONNECTION_WINDOW_HPP
#define CONNECTION_WINDOW_HPP

#include <array>
#include "imgui.h"
#include "serial.hpp"

#include <string>
#include <vector>

class ConnectionWindow
{

public:
    ConnectionWindow();
    ~ConnectionWindow();

    void update();

    std::string get_selected_port();
    int get_selected_index();

private:
    int _current_id = 0;
    std::vector<std::string> _com_ports;

    int _baud_rate = 115200;
    std::array<int, 6> _available_baud_rates = {9600, 19200, 38400, 57600, 115200, 230400};

    bool _port_open = false;
};

#endif // CONNECTION_WINDOW_HPP