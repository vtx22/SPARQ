#pragma once

#include "Window.hpp"

#include <array>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include "serial.hpp"

#include "ImGuiNotify.hpp"

class ConnectionWindow : public Window
{
public:
    ConnectionWindow(DataHandler* data_handler, Serial* sp);

    void update_content();

    [[nodiscard]]
    constexpr auto get_selected_port() const;

    [[nodiscard]]
    constexpr auto get_selected_index() const noexcept;

    constexpr auto update_com_ports_dropdown()
    {
        _com_ports = Serial::get_port_names();

        if (_com_ports.size() == 0)
        {
            _com_ports.push_back("COM-");
            return std::size_t{};
        }

        return _com_ports.size();
    }

private:
    std::size_t _current_id = 0;
    std::vector<std::string> _com_ports;

    unsigned int _baud_rate = 115'200;
    const std::array<int, 9> _available_baud_rates{4800u, 9600, 19'200, 38'400, 57'600, 115'200, 230'400, 460'800, 921'600};
    char _signature_chars[3] = {'F', 'F', 0};
    uint8_t _signature = 0xFF;

    bool _port_open = false;
    std::size_t _selected_comm_mode = 0;

    [[nodiscard]]
    static uint8_t hex_chars_to_byte(char high, char low) noexcept;

    Serial* _sp;
};
