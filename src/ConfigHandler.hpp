#pragma once

#include <iostream>
#include <string>

#include "implot.h"

#include "sparq_config.h"

#include "ImGuiNotify.hpp"
#include "mini/ini.h"

class ConfigHandler
{
public:
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
