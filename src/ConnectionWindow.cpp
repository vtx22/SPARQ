#include "ConnectionWindow.hpp"

ConnectionWindow::ConnectionWindow(/* args */)
{
}

ConnectionWindow::~ConnectionWindow()
{
}

void ConnectionWindow::update()
{
    if (ImGui::Begin("Connection"))
    {
        static std::vector<std::string> com_ports;

        if (ImGui::Button("Refresh Ports"))
        {
            com_ports = Serial::get_port_names();
        }

        if (com_ports.size() == 0)
        {
            com_ports.push_back("COM-");
        }

        static int item_current_idx = 0;

        if (ImGui::BeginCombo("COM Port", com_ports[item_current_idx].c_str()))
        {
            for (int n = 0; n < com_ports.size(); n++)
            {
                const bool is_selected = (item_current_idx == n);

                if (ImGui::Selectable(com_ports[n].c_str(), is_selected))
                {
                    item_current_idx = n;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
    ImGui::End();
}