#pragma once

#include "ImGuiNotify.hpp"
#include "Window.hpp"
#include "serial.hpp"

class ConnectionWindow final : public Window
{
public:
    ConnectionWindow(DataHandler& data_handler, Serial& sp)
        : Window(ICON_FA_NETWORK_WIRED "  Connection", data_handler),
          m_sp(sp)
    {
        update_com_ports_dropdown();
    }

private:
    [[nodiscard]]
    constexpr auto get_selected_port() const
    {
        return m_com_ports.at(m_current_id);
    }

    [[nodiscard]]
    constexpr auto get_selected_index() const noexcept
    {
        return m_current_id;
    }

    std::size_t update_com_ports_dropdown()
    {
        m_com_ports = Serial::get_port_names();

        if (m_com_ports.empty())
        {
            m_com_ports.emplace_back("COM-");
            return {};
        }

        return m_com_ports.size();
    }

    std::size_t m_current_id = 0;
    std::vector<std::string> m_com_ports;

    unsigned int m_baud_rate = 115'200;
    std::array<int, 9> const m_available_baud_rates{4800u, 9600, 19'200, 38'400, 57'600, 115'200, 230'400, 460'800, 921'600};
    char m_signature_chars[3] = {'F', 'F', 0};
    uint8_t m_signature = 0xFF;

    bool m_port_open = false;
    std::size_t m_selected_comm_mode = 0;

    Serial& m_sp;

protected:
    void update_content(Datasets& datasets) override
    {
        std::scoped_lock lock(m_data_handler.get_serial_mutex());

        ImGui::SeparatorText("Settings");
        m_port_open = m_sp.get_open();

        if (m_port_open)
        {
            ImGui::BeginDisabled();
        }

        auto const spacing_right = 4.5f * ImGui::GetFontSize();
        ImGui::SetNextItemWidth(-spacing_right);

        // COM Port selection
        if (ImGui::BeginCombo("##ComPorts", m_com_ports[m_current_id].c_str()))
        {
            for (std::size_t n = 0; n < m_com_ports.size(); n++)
            {
                auto const is_selected = (m_current_id == n);

                if (ImGui::Selectable(m_com_ports.at(n).c_str(), is_selected))
                {
                    m_current_id = n;
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
            update_com_ports_dropdown();
        }

        ImGui::SetNextItemWidth(-spacing_right);
        // Baud Rate selection
        if (ImGui::BeginCombo("###BaudRateSelect", std::to_string(m_baud_rate).c_str()))
        {
            for (auto const& rate : m_available_baud_rates)
            {
                auto const is_selected = m_baud_rate == rate;

                if (ImGui::Selectable(std::to_string(rate).c_str(), is_selected))
                {
                    m_baud_rate = rate;
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

        constexpr std::array comm_modes{"SPARQ", "ASCII"};
        if (ImGui::BeginCombo("###CommModeSelect", comm_modes[m_selected_comm_mode]))
        {
            for (std::size_t n = 0; n < comm_modes.size(); n++)
            {
                auto const is_selected = (n == m_selected_comm_mode);

                if (ImGui::Selectable(comm_modes[n], is_selected))
                {
                    m_selected_comm_mode = n;
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

        if (m_selected_comm_mode != 0)
        {
            ImGui::BeginDisabled();
        }
        ImGui::SetNextItemWidth(50);
        ImGui::InputText("##SignatureInput", m_signature_chars, 3, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
        ImGui::SameLine();
        ImGui::Text("Signature");

        if (m_selected_comm_mode != 0)
        {
            ImGui::EndDisabled();
        }

        ImGui::EndDisabled();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.7, 0, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0.3, 0, 1));
        if (ImGui::Button("Open"))
        {
            if (!(m_com_ports.size() == 1 && m_com_ports[0] == "COM-"))
            {
                m_signature = spq::helper::hex_chars_to_byte(m_signature_chars[0], m_signature_chars[1]);

                auto const selected_port = m_com_ports[m_current_id].c_str();
                std::cout << "Opening Port: " << selected_port << " (" << m_baud_rate << ") ..." << std::endl;

                auto const rtn = m_sp.open(selected_port, m_baud_rate);

                if (rtn == SERIAL_ERR::OK)
                {
                    std::cout << "Port opened successfully!\n";
                    m_port_open = true;
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
        else if (m_port_open)
        {
            ImGui::EndDisabled();
        }

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7, 0, 0, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3, 0, 0, 1));
        if (!m_port_open)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Close"))
        {
            std::cout << "Closing COM port ..." << std::endl;

            m_sp.close();

            ImGui::InsertNotification({ImGuiToastType::Success, SPARQ_NOTIFY_DURATION_OK, "COM port closed successfully!"});
            m_port_open = false;
        }
        else if (!m_port_open)
        {
            ImGui::EndDisabled();
        }
        ImGui::PopStyleColor(4);

        if (m_port_open)
        {
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0, 0.6, 1, 1));
            ImGui::ProgressBar(
                -0.6f * static_cast<float>(ImGui::GetTime()),
                ImVec2(-std::numeric_limits<float>::min(), 0.5f * ImGui::GetFontSize()),
                "");
            ImGui::PopStyleColor();
        }
    }
};
