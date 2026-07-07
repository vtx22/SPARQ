#pragma once

#include "ConfigHandler.hpp"
#include "DataHandler.hpp"
#include "IconsFontAwesome6.h"

#include "imgui.h"

class Window
{
public:
    Window(std::string name, DataHandler& data_handler)
        : m_window_name(std::move(name)),
          m_data_handler(data_handler),
          m_config_handler(ConfigHandler::get_instance())
    {
    }

    Window(std::string name, DataHandler& data_handler, ImGuiWindowFlags const flags)
        : m_window_name(std::move(name)),
          m_window_flags(flags),
          m_data_handler(data_handler),
          m_config_handler(ConfigHandler::get_instance())
    {
    }

    virtual ~Window() = default;

    /**
     * @brief Draws the window using ImGui. This function should be called every frame to handle and render the window.
     * @details It handles the ImGui::Begin() and ImGui::End() calls, as well as checking for user interaction with the close button.
     * @see Derived classes should implement the update_content() function to define the specific content of the window.
     */
    void draw()
    {
        before_imgui_begin();

        bool should_stay_open = true;
        if (ImGui::Begin(m_window_name.c_str(), has_close_button() ? &should_stay_open : nullptr, m_window_flags))
        {
            m_is_selected = ImGui::IsWindowFocused();

            auto const dataset_lock = m_data_handler.datasets();
            update_content(dataset_lock.get());
        }

        if (!should_stay_open)
        {
            m_close_triggered = true;
        }

        ImGui::End();

        after_imgui_end();
    }

    /**
     * @brief Checks if the window has been triggered to close by the user using the manual close button.
     * @return true if the window has been triggered to close, false otherwise.
     */
    [[nodiscard]]
    constexpr bool close_triggered() const noexcept
    {
        return m_close_triggered;
    }

    /**
     * @brief Checks if the window is currently selected (focused) using ImGui::IsWindowFocused().
     * @return true if the window is selected, false otherwise.
     */
    [[nodiscard]]
    constexpr bool is_selected() const noexcept
    {
        return m_is_selected;
    }

protected:
    /**
     * @brief Updates the content of the window. This function is called every frame between the windows ImGui::Begin() and ImGui::End() calls.
     * @details Derived classes should implement this function to define the specific content of the window.
     * @param datasets A reference to the datasets managed by the DataHandler, allowing derived classes to access and manipulate the data as needed.
     */
    virtual void update_content(Datasets& datasets) = 0;

    /**
     * @brief Called before the ImGui::Begin() call in the draw() function.
     * @details Derived classes can override this function to perform any necessary setup or state changes before the window is drawn.
     */
    virtual void before_imgui_begin()
    {
    }

    /**
     * @brief Called after the ImGui::End() call in the draw() function.
     * @details Derived classes can override this function to perform any necessary cleanup or state changes after the window is drawn.
     */
    virtual void after_imgui_end()
    {
    }

    /**
     * @brief Determines whether the window has a close button.
     * Derived classes can override this function to specify whether the window should have a close button or not.
     * @details This method is called in the draw() function to determine whether to pass a pointer to the should_stay_open variable to ImGui::Begin().
     * If this method returns true, the window will have a close button, and the should_stay_open variable will be updated based on user interaction.
     * If it returns false, the window will not have a close button, and the should_stay_open variable will not be used.
     * @see close_triggered() for checking if the window has been triggered to close by the user.
     * @return true if the window has a close button, false otherwise.
     */
    [[nodiscard]]
    constexpr virtual bool has_close_button() const noexcept
    {
        return false;
    }

    std::string m_window_name{};
    ImGuiWindowFlags m_window_flags{};
    DataHandler& m_data_handler;
    ConfigHandler& m_config_handler;

private:
    bool m_close_triggered{};
    bool m_is_selected{};
};
