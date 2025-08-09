#include <core/GameObject.hpp>
#include <core/Material.hpp>
#include <core/Log.hpp>

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

auto GameObject::Bind() const -> void
{
    gl::BindVertexArray(m_Mesh->VAO);
}

auto GameObject::Name() const -> std::string
{
    return m_Name;
}