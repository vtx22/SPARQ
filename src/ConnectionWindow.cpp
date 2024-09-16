#include "ConnectionWindow.hpp"

ConnectionWindow::ConnectionWindow(Serial *sp) : _sp(sp)
{
}

ConnectionWindow::~ConnectionWindow()
{
}

void ConnectionWindow::update()
{
    if (ImGui::Begin("Connection"))
    {
        ImGui::SeparatorText("Settings");

        if (_com_ports.size() == 0)
        {
            _com_ports.push_back("COM-");
        }

        if (_port_open)
        {
            ImGui::BeginDisabled();
        }

        // ImGui::PushItemWidth(-FLT_MIN);

        // COM Port selection
        if (ImGui::BeginCombo("##ComPorts", _com_ports[_current_id].c_str()))
        {
            for (int n = 0; n < (int)_com_ports.size(); n++)
            {
                bool is_selected = (_current_id == n);

                if (ImGui::Selectable(_com_ports.at(n).c_str(), is_selected))
                {
                    _current_id = n;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine();
        if (ImGui::Button("Refresh Ports"))
        {
            _com_ports = Serial::get_port_names();
        }

        // ImGui::PopItemWidth();

        // Baud Rate selection
        if (ImGui::BeginCombo("###BaudRateSelect", std::to_string(_baud_rate).c_str()))
        {
            for (uint8_t n = 0; n < _available_baud_rates.size(); n++)
            {
                bool is_selected = (_baud_rate == _available_baud_rates[n]);

                if (ImGui::Selectable(std::to_string(_available_baud_rates[n]).c_str(), is_selected))
                {
                    _baud_rate = _available_baud_rates[n];
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine();
        ImGui::Text("Baud Rate");

        ImGui::Checkbox("SPARQ Format", &_sparq_format);
        ImGui::SameLine();

        if (!_sparq_format)
        {
            ImGui::BeginDisabled();
        }

        ImGui::InputText("##SignatureInput", _signature_chars, 3, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
        ImGui::SameLine();
        ImGui::Text("Signature");

        if (!_sparq_format)
        {
            ImGui::EndDisabled();
        }

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 1));
        if (ImGui::Button("Open"))
        {
            _signature = hex_chars_to_byte(_signature_chars[0], _signature_chars[1]);
            if (_sp->open(_com_ports[_current_id].c_str(), _baud_rate) == SERIAL_ERR::OK)
            {
                _port_open = true;
            }
        }
        else if (_port_open)
        {
            ImGui::EndDisabled();
        }

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
        if (!_port_open)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Close"))
        {
            _sp->close();
            _port_open = false;
        }
        else if (!_port_open)
        {
            ImGui::EndDisabled();
        }
        ImGui::PopStyleColor(2);

        ImGui::SeparatorText("Statistic");

        ImGui::Text("FPS: %d", (int)ImGui::GetIO().Framerate);
    }
    ImGui::End();
}

int ConnectionWindow::get_selected_index()
{
    return _current_id;
}

std::string ConnectionWindow::get_selected_port()
{
    return _com_ports.at(_current_id);
}

uint8_t ConnectionWindow::hex_chars_to_byte(char high, char low)
{
    auto char_to_hex_value = [](char c) -> uint8_t
    {
        if (c == 0)
        {
            return 0;
        }
        if (c >= '0' && c <= '9')
        {
            return c - '0';
        }
        else if (c >= 'A' && c <= 'F')
        {
            return c - 'A' + 10;
        }
        else if (c >= 'a' && c <= 'f')
        {
            return c - 'a' + 10;
        }
        return 0;
    };

    uint8_t high_value = char_to_hex_value(high);
    uint8_t low_value = char_to_hex_value(low);

    return (high_value << 4) | low_value;
}