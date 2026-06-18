#include "Scene.hpp"
#include "Camera.hpp"
#include "GameObject.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include <core/Log.hpp>

#include <algorithm>

Scene::Scene()
    : m_Cameras({Camera()})
    , m_MainCamera(m_Cameras[0])
{}

auto Scene::entities() -> EntitiesVector&
{
    return m_Entities;
}

auto Scene::entities() const -> const EntitiesVector&
{
    return m_Entities;
}

auto Scene::clear() -> void
{
    m_Entities.clear();
}

auto Scene::operator<<(GameObject entity)-> Scene&
{
    m_Entities.emplace(std::move(entity));
    return *this;
}

auto Scene::operator<<(Camera cam)-> Scene&
{
    m_Cameras.emplace_back(std::move(cam));
    return *this;
}

auto Scene::main_camera() -> Camera&
{
    return m_MainCamera;
}

auto Scene::main_camera() const -> Camera&
{
    return m_MainCamera;
}