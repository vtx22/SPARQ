#include "ViewWindow.hpp"

ViewWindow::ViewWindow(DataHandler *data_handler) : _data_handler(data_handler)
{
}

ViewWindow::~ViewWindow()
{
}

void ViewWindow::update()
{
    if (ImGui::Begin(ICON_FA_SLIDERS "  View"))
    {

        if (ImGui::CollapsingHeader("Plot Type"))
        {
            menu_plot_type();
        }

        if (ImGui::CollapsingHeader("X Axis"))
        {
            menu_x_axis();
        }

        if (ImGui::CollapsingHeader("Y Axis"))
        {
            menu_y_axis();
        }
    }

    ImGui::End();
}

void ViewWindow::menu_plot_type()
{
    const char *plot_types[2] = {"Line", "Heatmap"};
    if (ImGui::BeginCombo("##PLOT TYPE", plot_types[(int)_data_handler->plot_settings.type]))
    {
        for (uint8_t n = 0; n < 2; n++)
        {
            bool is_selected = ((uint8_t)_data_handler->plot_settings.type == n);

            if (ImGui::Selectable(plot_types[n], is_selected))
            {
                _data_handler->plot_settings.type = (sparq_plot_t)n;
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (_data_handler->plot_settings.type == sparq_plot_t::HEATMAP)
    {
        auto &hms = _data_handler->plot_settings.heatmap_settings;
        ImGui::Checkbox("Equal", &hms.equal);
        ImGui::InputInt("##HMI_R", &hms.rows);
        ImGui::SameLine();
        ImGui::Text("x");
        ImGui::SameLine();
        ImGui::InputInt("###HMI_C", &hms.cols);
    }
}

void ViewWindow::menu_x_axis()
{
    const int text_offset = 90;
    ImGui::Text("Axis Unit");
    ImGui::SameLine();
    ImGui::SetCursorPosX(text_offset);
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::BeginCombo("##X View", x_axis_types[_data_handler->x_axis_select].dropdown_name))
    {
        for (uint8_t n = 0; n < x_axis_types.size(); n++)
        {
            bool is_selected = (_data_handler->x_axis_select == n);

            if (ImGui::Selectable(x_axis_types[n].dropdown_name, is_selected))
            {
                _data_handler->x_axis_select = n;
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Fit");
    ImGui::SameLine();
    ImGui::SetCursorPosX(text_offset);
    ImGui::SetNextItemWidth(-200);
    if (ImGui::BeginCombo("##X Fit", x_axis_fits[_data_handler->x_fit_select]))
    {
        for (uint8_t n = 0; n < x_axis_fits.size(); n++)
        {
            bool is_selected = (_data_handler->x_fit_select == n);

            if (ImGui::Selectable(x_axis_fits[n], is_selected))
            {
                _data_handler->x_fit_select = n;
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

    if (_data_handler->x_fit_select != 2)
    {
        ImGui::BeginDisabled();
    }

    ImGui::SetNextItemWidth(-210);
    ImGui::Text("N =");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(160);
    ImGui::InputInt("##LastN", &_data_handler->last_n, 1, 10);
    if (_data_handler->last_n < 2)
    {
        _data_handler->last_n = 2;
    }

    if (_data_handler->x_fit_select != 2)
    {
        ImGui::EndDisabled();
    }
}

void ViewWindow::menu_y_axis()
{
    const int text_offset = 90;
    ImGui::Text("Fit");
    ImGui::SameLine();
    ImGui::SetCursorPosX(text_offset);
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::BeginCombo("##Y Fit", y_axis_fits[_data_handler->y_fit_select]))
    {
        for (uint8_t n = 0; n < y_axis_fits.size(); n++)
        {
            bool is_selected = (_data_handler->y_fit_select == n);

            if (ImGui::Selectable(y_axis_fits[n], is_selected))
            {
                _data_handler->y_fit_select = n;
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