#include "ViewWindow.hpp"

void ViewWindow::update_content()
{
    if (ImGui::Button("Add Plot"))
    {
        _on_create_plotting_window();
    }

    ImGui::SeparatorText("Plot Settings");

    if (auto settings = _get_selected_plot_settings())
    {
        show_plot_settings(*settings);
    }

    auto& ds = _data_handler.get_datasets_editable();

    for (auto& d : ds)
    {
        ImGui::PushID(d.id); // In case there is a duplicate dataset name

        constexpr ImVec4 button_color{0.f, 0.5f, 1.f, 1.f};
        ImGui::PushStyleColor(ImGuiCol_Button, d.hidden ? ImVec4(0.f, 0.f, 0.f, 0.f) : button_color);
        ImGui::PushStyleColor(ImGuiCol_Border, button_color);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.f);

        if (ImGui::Button(d.name.c_str()))
        {
            d.toggle_visibility = true;
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(2);
        ImGui::PopID();
    }
}

void ViewWindow::show_plot_settings(spq::plotting::plot_settings& settings)
{
    using namespace spq::plotting;

    auto selected_plot_type = static_cast<int>(settings.type);
    if (ImGui::Combo("Type", &selected_plot_type, plot_type_names.data(), plot_type_names.size()))
    {
        settings.type = static_cast<plot_type>(selected_plot_type);
    }

    show_axis_settings(settings);

    switch (settings.type)
    {
    case plot_type::timeseries:
        break;
    case plot_type::single_value:
        break;
    case plot_type::heatmap:
        show_heatmap_settings(settings.heatmap_settings);
        break;
    default:
        break;
    }
}

void ViewWindow::show_heatmap_settings(spq::plotting::heatmap_settings& settings)
{
}

void ViewWindow::show_axis_settings(spq::plotting::plot_settings& settings)
{
    using namespace spq::plotting;

    auto const spacing_right = 3.5f * ImGui::GetFontSize();
    ImGui::PushItemWidth(-spacing_right);

    if (ImGui::CollapsingHeader("X Axis"))
    {
        auto selected_x_fit_type = static_cast<int>(settings.x_fit);
        if (ImGui::Combo("X Fit", &selected_x_fit_type, x_fit_names.data(), x_fit_names.size()))
        {
            settings.x_fit = static_cast<x_fit>(selected_x_fit_type);
        }
    }

    if (ImGui::CollapsingHeader("Y Axis"))
    {
        auto selected_y_fit_type = static_cast<int>(settings.y_fit);
        if (ImGui::Combo("Y Fit", &selected_y_fit_type, y_fit_names.data(), y_fit_names.size()))
        {
            settings.y_fit = static_cast<y_fit>(selected_y_fit_type);
        }
    }

    ImGui::PopItemWidth();
}
