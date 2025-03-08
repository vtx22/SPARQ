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
    std::cout << std::endl;

    std::cout << "Loading " << SPARQ_CONFIG_FILE << " ..." << std::endl;

    mINI::INIFile _ini_file(SPARQ_CONFIG_FILE);
    if (!_ini_file.read(ini))
    {
        std::cerr << "Failed to load config.ini!" << std::endl;
        ImGui::InsertNotification({ImGuiToastType::Error, SPARQ_NOTIFY_DURATION_ERR, "Failed to read config.ini!"});
    }

    std::cout << "Settings:\n";
    std::cout << "Graphics Settings:\n";
    std::cout << "    Antialiasing:     " << std::stoi(ini["graphics"]["antialiasing"]) << "\n";
    std::cout << "    VSync:            " << ((ini["graphics"]["vsync"] == "1") ? "Enabled" : "Disabled") << "\n";
    std::cout << "    FPS Limit:        " << SPARQ_MAX_FPS << "\n";

    std::cout << "Downsampling Settings:\n";
    std::cout << "    Enabled:          " << ((ini["downsampling"]["enabled"] == "1") ? "True" : "False") << "\n";
    std::cout << "    Mode:             " << std::stoi(ini["downsampling"]["mode"]) << "\n";
    std::cout << "    Max Samples:      " << std::stoi(ini["downsampling"]["max_samples"]) << "\n";
    std::cout << "    Max Samples Type: " << std::stoi(ini["downsampling"]["max_samples_type"]) << "\n";

    std::cout << "Color Settings:\n";
    auto cm = (ImPlotColormap)std::stoi(ini["color"]["colormap"]);
    std::cout << "    Colormap:         " << cm << "\n";

    std::cout << std::endl;
}

void ConfigHandler::write_config()
{
    std::cout << "Writing " << SPARQ_CONFIG_FILE << " ..." << std::endl;

    mINI::INIFile _ini_file(SPARQ_CONFIG_FILE);
    if (!_ini_file.write(ini))
    {
        std::cerr << "Failed to write config.ini!" << std::endl;
        ImGui::InsertNotification({ImGuiToastType::Error, SPARQ_NOTIFY_DURATION_ERR, "Failed to write config.ini!"});
    }
}

void ConfigHandler::apply_in_context_settings()
{
    ImPlot::GetStyle().Colormap = (ImPlotColormap)std::stoi(ini["color"]["colormap"]);
}