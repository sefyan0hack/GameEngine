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
    objs.emplace_back(gobj);
}

auto Scene::GetGameObjects() -> std::vector<GameObject>&
{
    return objs;
}