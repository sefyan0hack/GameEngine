#include <core/Scene.hpp>
#include <core/GameObject.hpp>
#include <vector>

Scene::Scene(std::unique_ptr<SkyBox> skybox)
    : m_SkyBox(std::move(skybox))
{}

Scene::~Scene()
{
}

auto Scene::add(GameObject gobj) -> void
{
    
    m_Entitys.emplace_back(std::move(gobj));
}

auto Scene::Entitys() -> std::vector<GameObject>&
{
    return m_Entitys;
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