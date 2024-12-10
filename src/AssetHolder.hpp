#pragma once

#include <cstring>
#include <array>
#include <iostream>
#include <cstdint>
#include <filesystem>

#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"

#include "imgui.h"

#include "sparq_config.h"

class AssetHolder
{
public:
    AssetHolder();

    void add_all_assets();
    void add_asset(const char *path);
    ImTextureID get_handle(std::string name);

    static ImTextureID gl_handle_to_imgui_id(GLuint gl_texture_handle);

private:
    std::string extract_filename(const char *file_path);

    std::array<sf::Texture, SPARQ_MAX_TEXTURES> _textures;
    std::array<std::string, SPARQ_MAX_TEXTURES> _names;
    std::array<ImTextureID, SPARQ_MAX_TEXTURES> _texture_ids;
    int stored_textures = 0;
};