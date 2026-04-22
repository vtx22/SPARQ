#include "ViewWindow.hpp"

void ViewWindow::update_content()
{
    if (ImGui::Button("Add Plot"))
    {
        if (_on_create_plotting_window)
        {
            _on_create_plotting_window();
        }
    }

    ImGui::SeparatorText("Plot Settings");

    if (_get_selected_plot_settings)
    {
        if (auto settings = _get_selected_plot_settings())
        {
            show_plot_settings(settings.value());
            return;
        }
    }
}

void ViewWindow::show_plot_settings(spq::plotting::plot_settings& settings)
{
    using namespace spq::plotting;

    auto selected_plot_type = static_cast<int>(settings.type);
    if (ImGui::Combo("Type", &selected_plot_type, spq::plotting::plot_type_names.data(), spq::plotting::plot_type_names.size()))
    {
        settings.type = static_cast<plot_type>(selected_plot_type);
    }
}
