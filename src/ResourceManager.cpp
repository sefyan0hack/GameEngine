#include <core/ResourceManager.hpp>


ResourceManager::~ResourceManager()
{
}


auto ResourceManager::load(const std::string& path, ResType type) -> void
{
    //bug maybe in future `when a texture2d hase name of texturecube without _dir.ext`
    switch (type)
    {
        case ResType::Texture2D:
        {
            m_Textures[path] = std::make_shared<Texture2D>(path);
            break;
        }
        case ResType::TextureCube:
        {
            // Error("{}", VecWrapper{strig_to_vector(path)});
            m_Textures[path] = std::make_shared<TextureCubeMap>(TextureCubeMap::base_to_6faces(path));
            break;
        }
        default: Error("Unsupported Resource Type");
    }
}

auto ResourceManager::getTexture(const std::string& path ) -> std::shared_ptr<Texture>
{
    
    return m_Textures[path];
}

auto ResourceManager::getInstance() -> ResourceManager&
{
    static ResourceManager S_instance;
    return S_instance;
}