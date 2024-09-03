#ifndef CONNECTION_WINDOW_HPP
#define CONNECTION_WINDOW_HPP

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
};

#endif // CONNECTION_WINDOW_HPP