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

auto GameObject::SetUp(std::vector<glm::vec3> InsPos) -> void
{
    m_InstancePos = std::move(InsPos);
    auto size = m_InstancePos.size();
    using VetexData = typename std::remove_cv_t<typename std::remove_reference_t<decltype(m_InstancePos)>::value_type>;

    auto currentVAO = Mesh::CurrentVAO();
    if(size > 1){
        GLuint VBO; // posible leak vbo think about it `TODO`
        gl::GenBuffers(1, &VBO);
        
        Expect(m_Mesh->VAO != 0, "VAO is 0");
        auto currentVBO = m_Mesh->CurrentVBO();

        gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
        gl::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size * sizeof(glm::vec3)), m_InstancePos.data(), GL_STATIC_DRAW);
        gl::BindBuffer(GL_ARRAY_BUFFER, currentVBO);

        AttributeInfo positions{
            VetexData::length(),
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::vec3),
            0,
            1,
        };
    
        m_Mesh->setAttribute(3, positions); // dnt forget  3 is the index need better way
        gl::BindVertexArray(currentVAO);
    }
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

auto GameObject::InstancePos() const -> const std::vector<glm::vec3> &
{
    return m_InstancePos;
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