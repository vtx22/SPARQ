#include "ConfigHandler.hpp"

ConfigHandler &ConfigHandler::get_instance()
{
    static ConfigHandler instance;
    return instance;
}

ConfigHandler::ConfigHandler()
{
    read_config();
}

void ConfigHandler::read_config()
{
    mINI::INIFile _ini_file(SPARQ_CONFIG_FILE);
    if (!_ini_file.read(ini))
    {
        ImGui::InsertNotification({ImGuiToastType::Error, 3000, "Failed to read config.ini!"});
    }
}

void ConfigHandler::write_config()
{
    mINI::INIFile _ini_file(SPARQ_CONFIG_FILE);
    if (!_ini_file.write(ini))
    {
        ImGui::InsertNotification({ImGuiToastType::Error, 3000, "Failed to write config.ini!"});
    }
}