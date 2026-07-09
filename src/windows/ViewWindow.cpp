#include "ViewWindow.hpp"

#include "plotting/PlotSettings.hpp"

namespace spq::ui
{
    void ViewWindow::update_content(data::Datasets& datasets)
    {
        if (ImGui::Button("Add Plot"))
        {
            ImGui::OpenPopup("plot_select_popup");
        }
        if (ImGui::BeginPopup("plot_select_popup"))
        {
            for (uint8_t i{}; auto const& name : plotting::plot_type_names)
            {
                if (ImGui::Selectable(name))
                {
                    m_on_create_plotting_window(static_cast<plotting::plot_type>(i));
                }

                i++;
            }
            ImGui::EndPopup();
        }

        ImGui::SeparatorText("Plot Settings");

        auto plot_data = m_get_selected_plot_data();

        if (!plot_data)
        {
            ImGui::Text("Select a plotting window to edit its settings.");
            return;
        }

        auto& [ids_to_plot, plot_settings] = *plot_data;

        plot_settings.show_settings();
        show_dataset_selection(ids_to_plot, datasets);
    }

    void ViewWindow::show_dataset_selection(std::unordered_set<std::size_t>& ids_to_plot, data::Datasets& datasets)
    {
        for (auto& d : datasets.data())
        {
            constexpr ImVec4 button_color{0.f, 0.5f, 1.f, 1.f};
            auto const button_state_color = ids_to_plot.contains(d.id) ? button_color : ImVec4{};

            ImGui::PushStyleColor(ImGuiCol_Button, button_state_color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_state_color);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_state_color);
            ImGui::PushStyleColor(ImGuiCol_Border, button_color);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.f);

            if (ImGui::Button(d.name_with_id.c_str()))
            {
                helper::add_or_remove_from_set(ids_to_plot, d.id);
            }

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(4);
        }
    }
}
