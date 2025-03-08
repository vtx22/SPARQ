#pragma once

#include <string>
#include <iostream>

#include "implot.h"

#include "sparq_config.h"

#include "mini/ini.h"
#include "ImGuiNotify.hpp"

class ConfigHandler
{
public:
    static ConfigHandler &get_instance();

    ConfigHandler(ConfigHandler const &) = delete;
    ConfigHandler &operator=(ConfigHandler const &) = delete;

    mINI::INIStructure ini;

    void read_config();
    void write_config();

    void apply_in_context_settings();

private:
    ConfigHandler();
};