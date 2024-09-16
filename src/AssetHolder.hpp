#pragma once

#include <cstring>
#include <array>
#include <iostream>

#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"

#include "imgui.h"

#include "sparq_config.h"

class AssetHolder
{
public:
    AssetHolder();

    ImTextureID add_asset(const char *path);

    static ImTextureID gl_handle_to_imgui_id(GLuint gl_texture_handle);

private:
    std::array<sf::Texture, SPARQ_MAX_TEXTURES> _textures;
    int stored_textures = 0;
};