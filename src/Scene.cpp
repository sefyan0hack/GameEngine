#include <core/Scene.hpp>
#include <core/GameObject.hpp>
#include <vector>

Scene::Scene(/* args */)
{
}

Scene::~Scene()
{
}

auto Scene::add(const GameObject &gobj) -> void
{
    m_Entitys.emplace_back(gobj);
}

auto Scene::Entitys() -> std::vector<GameObject>&
{
    return m_Entitys;
}