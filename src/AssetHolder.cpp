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
        std::cout << "Could not load texture because maximum number of textures exceeded! Check sparq_config.h\n";
        return nullptr;
    }

    _names[stored_textures] = extract_filename(path);
    _textures[stored_textures] = texture;
    _textures[stored_textures].setSmooth(false); // Workaround to force OpenGL to load the texture for a valid handle
    GLuint handle = _textures[stored_textures].getNativeHandle();

    stored_textures++;
    return gl_handle_to_imgui_id(handle);
}

ImTextureID AssetHolder::get_handle(std::string name)
{
    for (uint32_t i = 0; i < _names.size(); i++)
    {
        if (_names[i] == name)
        {
            GLuint handle = _textures[i].getNativeHandle();
            return gl_handle_to_imgui_id(handle);
        }
    }

    return nullptr;
}

std::string AssetHolder::extract_filename(const char *file_path)
{
    std::string path(file_path);

    // Find the last directory separator (platform-independent)
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        // Extract and return the filename
        return path.substr(pos + 1);
    }

    // If no separator found, the entire path is the filename
    return path;
}