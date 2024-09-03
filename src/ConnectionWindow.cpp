#include "ConnectionWindow.hpp"

ConnectionWindow::ConnectionWindow()
{
}

ConnectionWindow::~ConnectionWindow()
{
}

void ConnectionWindow::update()
{
    // ImGui::SetNextWindowSizeConstraints(ImVec2(1000.0f, 0.0f), ImVec2(FLT_MAX, FLT_MAX));
    if (ImGui::Begin("Connection"))
    {

        if (_com_ports.size() == 0)
        {
            _com_ports.push_back("COM-");
        }

        if (ImGui::Button("Refresh Ports"))
        {
            _com_ports = Serial::get_port_names();
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);

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

        if (true)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Open"))
        {
        }

        if (true)
        {
            ImGui::EndDisabled();
        }
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
