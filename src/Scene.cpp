#include <core/Scene.hpp>
#include <core/GameObject.hpp>

Scene::Scene()
    : m_SkyBox()
{}

Scene::~Scene()
{
}

auto Scene::Entities() -> std::vector<GameObject>&
{
    return m_Entities;
}


auto Scene::setSkyBox(const std::string& BasePathName) -> void
{
    m_SkyBox = std::make_unique<SkyBox>(BasePathName);
}

auto Scene::skyBox() -> std::unique_ptr<SkyBox>&
{
    return m_SkyBox;
}

auto Scene::operator<<(GameObject obj) -> void
{
    m_Entities.emplace_back(std::move(obj));
}


auto Scene::to_json() const -> std::string
{
    return std::format("{}", *this);
}