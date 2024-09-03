#include "ConnectionWindow.hpp"

ConnectionWindow::ConnectionWindow()
{
}

ConnectionWindow::~ConnectionWindow()
{
}

void ConnectionWindow::update()
{
    if (ImGui::Begin("Connection"))
    {
        if (_com_ports.size() == 0)
        {
            _com_ports.push_back("COM-");
        }

        if (_port_open)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Refresh Ports"))
        {
            _com_ports = Serial::get_port_names();
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);

        // COM Port selection
        if (ImGui::BeginCombo("##", _com_ports[_current_id].c_str()))
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
        ImGui::PopItemWidth();

        // Baud Rate selection
        if (ImGui::BeginCombo("Baud Rate", std::to_string(_baud_rate).c_str()))
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

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 1));
        if (ImGui::Button("Open"))
        {
            _port_open = true;
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
            _port_open = false;
        }
        else if (!_port_open)
        {
            ImGui::EndDisabled();
        }
        ImGui::PopStyleColor(2);
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
