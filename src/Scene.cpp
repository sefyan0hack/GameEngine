#include <core/Scene.hpp>
#include <core/GameObject.hpp>

Scene::Scene(std::unique_ptr<SkyBox> skybox)
    : m_SkyBox(std::move(skybox))
{}

Scene::~Scene()
{
}

auto Scene::add(GameObject gobj) -> void
{
    
    m_Entities.emplace_back(std::move(gobj));
}

auto Scene::Entities() -> std::vector<GameObject>&
{
    return m_Entities;
}

auto Scene::setSkyBox(std::unique_ptr<SkyBox> skybox) -> void
{
    m_SkyBox = std::move(skybox);
}

auto Scene::skyBox() -> std::unique_ptr<SkyBox>&
{
    return m_SkyBox;
}

auto Scene::operator<<(GameObject obj) -> void
{
    add(std::move(obj));
}
auto Scene::to_json() const -> std::string
{
    return std::format("{}", *this);
}