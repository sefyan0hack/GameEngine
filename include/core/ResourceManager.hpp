#pragma once

#include <core/Texture.hpp>
#include <core/Shader.hpp>
#include <core/Log.hpp>


class ResourceManager
{
public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;


    template <class T>
    inline auto load(const std::string &path) -> void
    {
        m_Resources[path] = std::make_shared<T>(path);
    }

    template <class T>
    auto get(const std::string &path) -> std::shared_ptr<T>
    {
        return std::any_cast<std::shared_ptr<T>>(m_Resources.at(path));
    }

    static auto getInstance() -> ResourceManager&
    {
        static ResourceManager S_instance;
        return S_instance;
    }

private:
    // Make the constructor and destructor private to restrict instantiation
    ResourceManager() = default;
    ~ResourceManager() = default;

    std::unordered_map<std::string, std::any> m_Resources;

    FOR_TEST
};

inline static ResourceManager& ResManager = ResourceManager::getInstance();