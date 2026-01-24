#include <core/Log.hpp>
#include "GameObject.hpp"
#include "Material.hpp"
#include "Mesh.hpp"

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

GameObject::GameObject(glm::vec3 position, std::shared_ptr<Material> matt, std::shared_ptr<Mesh> mesh) noexcept
    : m_Transform(Transform(position))
    , m_Material(matt)
    , m_Mesh(mesh)
{CTOR_LOG}

GameObject::GameObject(Transform transform, std::shared_ptr<Material> matt, std::shared_ptr<Mesh> mesh) noexcept
    : m_Transform(transform)
    , m_Material(matt)
    , m_Mesh(mesh)
{CTOR_LOG}

GameObject::~GameObject()
{DTOR_LOG}


GameObject::GameObject(GameObject&& other) noexcept
    : m_Transform(std::move(other.m_Transform))
    , m_Material(other.m_Material)
    , m_Mesh(other.m_Mesh)  
{
    other.m_Material.reset();
    other.m_Mesh.reset();
}

auto GameObject::operator=(GameObject&& other) noexcept -> GameObject&
{
    if(this != &other){
        m_Transform = std::move(other.m_Transform);
        m_Material = std::move(other.m_Material);
        m_Mesh = std::move(other.m_Mesh);
    }
    return *this;
}

auto GameObject::transform() const -> Transform
{
    return m_Transform;
}

auto GameObject::model() const -> glm::mat4
{
    return m_Transform;
}

auto GameObject::set_position(const glm::vec3 &pos) -> void
{
    m_Transform.position = pos;
}

auto GameObject::set_scale(const glm::vec3 &Scale) -> void
{
    m_Transform.scale = Scale;
}

auto GameObject::rotate(float angle, glm::vec3 axis) -> void
{
    m_Transform.rotation += axis * angle;
}


auto GameObject::mesh() const -> std::shared_ptr<Mesh>
{
    return m_Mesh;
}

auto GameObject::material() const -> std::shared_ptr<Material>
{
    return m_Material;
}
