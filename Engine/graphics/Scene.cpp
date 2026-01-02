#include "Scene.hpp"
#include "Camera.hpp"
#include "GameObject.hpp"
#include "SkyBox.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "OpenGL.hpp"
#include <core/Log.hpp>

Scene::Scene()
    : m_Cameras({Camera()})
    , m_MainCamera(m_Cameras[0])
{ CTOR_LOG }

auto Scene::entities() const -> std::span<const GameObject>
{
    return m_Entities;
}

auto Scene::clear() -> void
{
    m_Entities.clear();
}

auto Scene::operator<<(GameObject entity)-> Scene&
{
    m_Entities.emplace_back(std::move(entity));
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