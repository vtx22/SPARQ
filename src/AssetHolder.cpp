#include "AssetHolder.hpp"

AssetHolder::AssetHolder() {}

ImTextureID AssetHolder::gl_handle_to_imgui_id(GLuint gl_texture_handle)
{
    ImTextureID textureID = nullptr;
    std::memcpy(&textureID, &gl_texture_handle, sizeof(GLuint));
    return textureID;
}

ImTextureID AssetHolder::add_asset(const char *path)
{
    sf::Texture texture;
    if (!texture.loadFromFile(path))
    {
        std::cout << "Could not load texture " << path << "\n";
        return nullptr;
    }

    if (stored_textures >= SPARQ_MAX_TEXTURES)
    {
        std::cout << "Could not load texture because maximum number of textured exceeded! Check sparq_config.h\n";
        return nullptr;
    }

    _textures[stored_textures] = texture;
    _textures[stored_textures].setSmooth(false); // Workaround to force OpenGL to load the texture for a valid handle
    GLuint handle = _textures[stored_textures].getNativeHandle();

    stored_textures++;
    return gl_handle_to_imgui_id(handle);
}