#pragma once

#include <core/Texture.hpp>
#include <core/Shader.hpp>
#include <core/Log.hpp>

enum class ResType{
    Texture2D,
    TextureCube,
};

class ResourceManager
{
public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    static auto getInstance() -> ResourceManager&;

    auto load(const std::string& path, ResType type) -> void;
    auto getTexture(const std::string& path) -> std::shared_ptr<Texture>;

private:
    // Make the constructor and destructor private to restrict instantiation
    ResourceManager() = default;
    ~ResourceManager();

    std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;

    FOR_TEST
};

auto strig_to_vector(const std::string& path) -> std::vector<std::string>;

inline static ResourceManager& ResManager = ResourceManager::getInstance();