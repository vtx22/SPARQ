#pragma once

#include "ImGuiNotify.hpp"
#include "implot.h"
#include "mini/ini.h"
#include "sparq_config.h"

class ConfigHandler
{
public:
    [[nodiscard]]
    static ConfigHandler& get_instance();

    ConfigHandler(ConfigHandler const&) = delete;
    ConfigHandler& operator=(ConfigHandler const&) = delete;

    mINI::INIStructure ini;

    void read_config();
    void write_config();
    void apply_in_context_settings();

private:
    ConfigHandler();
};
