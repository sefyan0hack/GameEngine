#include "GameObject.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Log.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameObject::GameObject(glm::vec3 position, std::shared_ptr<Material> matt, std::shared_ptr<Mesh> mesh, std::string Name)
    : m_Transform(Transform(position))
    , m_Material(matt)
    , m_Mesh(mesh)
    , m_Name(Name)
{    
    Count++;
}

GameObject::GameObject(Transform transform, std::shared_ptr<Material> matt, std::shared_ptr<Mesh> mesh, std::string Name)
    : m_Transform(transform)
    , m_Material(matt)
    , m_Mesh(mesh)
    , m_Name(Name)
{    
    Count++;
}

GameObject::~GameObject()
{
}


GameObject::GameObject(GameObject&& other) noexcept
    : m_Transform(std::exchange(other.m_Transform, {}))
    , m_Material(std::exchange(other.m_Material, nullptr))
    , m_Mesh(std::exchange(other.m_Mesh, nullptr))
    , m_Name(std::exchange(other.m_Name, {}))
{
}

auto GameObject::operator=(GameObject&& other) noexcept -> GameObject&
{
    Debug::Print("Move {}", Count);

    if(this != &other){
        m_Transform = std::exchange(other.m_Transform, {});
        m_Material = std::exchange(other.m_Material, nullptr);
        m_Mesh = std::exchange(other.m_Mesh, nullptr);
        m_Name = std::exchange(other.m_Name, {});
    }
    return *this;
}

auto GameObject::transform() const -> Transform
{
    return m_Transform;
}

auto GameObject::Model() const -> glm::mat4
{
    return m_Transform;
}

auto GameObject::SetPosition(const glm::vec3 &pos) -> void
{
    m_Transform.position = pos;
}

auto GameObject::SetScale(const glm::vec3 &Scale) -> void
{
    m_Transform.scale = Scale;
}

auto GameObject::Rotate(float angle, glm::vec3 axis) -> void
{
    auto transformation = glm::mat4(1.0f); // wrong
    m_Transform = glm::rotate((transformation * (glm::mat4)m_Transform), glm::radians(angle), axis);
}


auto GameObject::mesh() const -> std::shared_ptr<Mesh>
{
    return m_Mesh;
}

auto GameObject::material() const -> std::shared_ptr<Material>
{
    return m_Material;
}


auto GameObject::Name() const -> std::string
{
    return m_Name;
}