#include <core/Scene.hpp>
#include <core/GameObject.hpp>

Scene::Scene()
    : m_SkyBox(std::make_unique<class SkyBox>())
{}

Scene::~Scene()
{
}


auto Scene::operator<<(GameObject obj) -> void
{
    m_Entities.emplace_back(std::move(obj));
}

auto Scene::Entities() const -> std::span<const GameObject>
{
    return m_Entities;
}

auto Scene::SetSkyBox(const std::string& BasePathName) -> void
{
    m_SkyBox = std::make_unique<class SkyBox>(BasePathName);
}

auto Scene::SetSkyBox(std::shared_ptr<TextureCubeMap> texture) -> void
{
    m_SkyBox = std::make_unique<class SkyBox>(texture);
}

auto Scene::SkyBox() const -> const std::unique_ptr<class SkyBox>&
{
    return m_SkyBox;
}

auto Scene::Clear() -> void
{
    m_Entities.clear();
}