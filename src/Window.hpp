#pragma once

#include "ConfigHandler.hpp"
#include "DataHandler.hpp"
#include "IconsFontAwesome6.h"
#include "imgui.h"

class Window
{
public:
    Window(std::string name, DataHandler& data_handler)
        : _name(name),
          _data_handler(data_handler),
          _config_handler(ConfigHandler::get_instance())
    {
    }

    Window(std::string name, DataHandler& data_handler, ImGuiWindowFlags flags)
        : window_flags(flags),
          _name(name),
          _data_handler(data_handler),
          _config_handler(ConfigHandler::get_instance())
    {
    }

    virtual ~Window() = default;

    void draw()
    {
        before_imgui_begin();

        bool should_stay_open = true;
        if (ImGui::Begin(_name.c_str(), has_close_button() ? &should_stay_open : nullptr, window_flags))
        {
            _is_selected = ImGui::IsWindowFocused();
            update_content();
        }

        if (!should_stay_open)
        {
            _close_triggered = true;
        }

        ImGui::End();

        after_imgui_end();
    }

    [[nodiscard]]
    constexpr bool close_triggered() const noexcept
    {
        return _close_triggered;
    }

    [[nodiscard]]
    constexpr bool is_selected() const noexcept
    {
        return _is_selected;
    }

protected:
    virtual void update_content() = 0;

    virtual void before_imgui_begin()
    {
    }

    virtual void after_imgui_end()
    {
    }

    [[nodiscard]]
    constexpr virtual bool has_close_button() const noexcept
    {
        return false;
    }

    std::string _name;
    DataHandler& _data_handler;
    ConfigHandler& _config_handler;
    ImGuiWindowFlags window_flags{};

private:
    bool _close_triggered{};
    bool _is_selected{};
};
