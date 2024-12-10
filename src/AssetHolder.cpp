#include "AssetHolder.hpp"

AssetHolder::AssetHolder()
{
    add_all_assets();
}

ImTextureID AssetHolder::gl_handle_to_imgui_id(GLuint gl_texture_handle)
{
    ImTextureID textureID = nullptr;
    std::memcpy(&textureID, &gl_texture_handle, sizeof(GLuint));
    return textureID;
}

void AssetHolder::add_all_assets()
{
    try
    {
        for (const auto &entry : std::filesystem::directory_iterator("./assets/"))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".png")
            {
                std::cout << "Loading asset " << entry.path().string() << " ...\n";
                add_asset(entry.path().string().c_str());
            }
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "Error accessing assets: " << e.what() << '\n';
    }
}

void AssetHolder::add_asset(const char *path)
{
    sf::Texture texture;
    if (!texture.loadFromFile(path))
    {
        std::cout << "Could not load texture " << path << "\n";
        return;
    }

    if (stored_textures >= SPARQ_MAX_TEXTURES)
    {
        std::cout << "Could not load texture because maximum number of textures exceeded! Check sparq_config.h\n";
        return;
    }

    _names[stored_textures] = extract_filename(path);
    _textures[stored_textures] = texture;
    _textures[stored_textures].setSmooth(false); // Workaround to force OpenGL to load the texture for a valid handle
    GLuint handle = _textures[stored_textures].getNativeHandle();
    _texture_ids[stored_textures] = gl_handle_to_imgui_id(handle);

    stored_textures++;
}

ImTextureID AssetHolder::get_handle(std::string name)
{
    for (uint32_t i = 0; i < _names.size(); i++)
    {
        if (_names[i] == name)
        {
            return _texture_ids[i];
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