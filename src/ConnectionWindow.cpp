#include "ConnectionWindow.hpp"

ConnectionWindow::ConnectionWindow(DataHandler *data_handler, Serial *sp) : Window(ICON_FA_NETWORK_WIRED "  Connection", data_handler), _sp(sp)
{
    _com_ports.push_back("COM-");
}

void ConnectionWindow::update_content()
{
    std::lock_guard<std::mutex> lock(_data_handler->get_serial_mutex());

    ImGui::SeparatorText("Settings");
    _port_open = _sp->get_open();

    if (_port_open)
    {
        ImGui::BeginDisabled();
    }

    float spacing_right = 4.5f * ImGui::GetFontSize();
    ImGui::SetNextItemWidth(-spacing_right);

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

    if (ImGui::Button(ICON_FA_ARROWS_ROTATE "##RefreshButton"))
    {
        _com_ports = Serial::get_port_names();

        if (_com_ports.size() == 0)
        {
            _com_ports.push_back("COM-");
        }
    }

    ImGui::SetNextItemWidth(-spacing_right);
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
    ImGui::SetNextItemWidth(spacing_right);

    ImGui::Text("Baud Rate");

    ImGui::SetNextItemWidth(-spacing_right - 50 - ImGui::GetStyle().ItemInnerSpacing.x);

    ImGui::BeginDisabled();

    const char *comm_modes[2] = {"SPARQ", "ASCII"};
    if (ImGui::BeginCombo("###CommModeSelect", comm_modes[_selected_comm_mode]))
    {
        for (uint8_t n = 0; n < 2; n++)
        {
            bool is_selected = (n == _selected_comm_mode);

            if (ImGui::Selectable(comm_modes[n], is_selected))
            {
                _selected_comm_mode = n;
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

    if (_selected_comm_mode != 0)
    {
        ImGui::BeginDisabled();
    }
    ImGui::SetNextItemWidth(50);
    ImGui::InputText("##SignatureInput", _signature_chars, 3, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
    ImGui::SameLine();
    ImGui::Text("Signature");

    if (_selected_comm_mode != 0)
    {
        ImGui::EndDisabled();
    }

    ImGui::EndDisabled();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.7, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0.3, 0, 1));
    if (ImGui::Button("Open"))
    {
        if (!(_com_ports.size() == 1 && _com_ports[0] == "COM-"))
        {
            _signature = hex_chars_to_byte(_signature_chars[0], _signature_chars[1]);

            const char *selected_port = _com_ports[_current_id].c_str();
            std::cout << "Opening Port: " << selected_port << " (" << _baud_rate << ") ..." << std::endl;

            int rtn = _sp->open(selected_port, _baud_rate);

            if (rtn == SERIAL_ERR::OK)
            {
                std::cout << "Port opened successfully!\n";
                _port_open = true;
                ImGui::InsertNotification({ImGuiToastType::Success, SPARQ_NOTIFY_DURATION_OK, "COM port opened successfully!"});
            }
            else
            {
                std::cerr << "Failed to open port! Error: " << rtn << "\n";
                ImGui::InsertNotification({ImGuiToastType::Error, SPARQ_NOTIFY_DURATION_ERR, "Could not open COM port!"});
            }
        }
        else
        {
            ImGui::InsertNotification({ImGuiToastType::Error, SPARQ_NOTIFY_DURATION_ERR, "No COM port selected!"});
        }
    }
    else if (_port_open)
    {
        ImGui::EndDisabled();
    }

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7, 0, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3, 0, 0, 1));
    if (!_port_open)
    {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button("Close"))
    {
        std::cout << "Closing COM port ..." << std::endl;

        _sp->close();

        ImGui::InsertNotification({ImGuiToastType::Success, SPARQ_NOTIFY_DURATION_OK, "COM port closed successfully!"});
        _port_open = false;
    }
    else if (!_port_open)
    {
        ImGui::EndDisabled();
    }
    ImGui::PopStyleColor(4);

    if (_port_open)
    {
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0, 0.6, 1, 1));
        ImGui::ProgressBar(-0.6f * (float)ImGui::GetTime(), ImVec2(-FLT_MIN, 0.5f * ImGui::GetFontSize()), "");
        ImGui::PopStyleColor();
    }
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