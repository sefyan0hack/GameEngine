#include <core/GameObject.hpp>
#include <core/Material.hpp>
#include <core/Log.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameObject::GameObject(glm::vec3 position, Material& matt, Mesh& mesh, std::string Name)
    : m_Transform(Transform(position))
    , m_Material(std::make_shared<Material>(matt))
    , m_Mesh(std::make_shared<Mesh>(mesh))
    , m_Name(Name)
{   
    UpMatrix();
    Info("{}", *this);
    Count++;
}

GameObject::GameObject(Transform transform, Material &matt, Mesh &mesh, std::string Name)
    : m_Transform(transform)
    , m_Material(std::make_shared<Material>(matt))
    , m_Mesh(std::make_shared<Mesh>(mesh))
    , m_Name(Name)
{
    UpMatrix();
    Info("{}", *this);
    Count++;
}

GameObject::~GameObject()
{
}

auto GameObject::UpMatrix() -> void
{
    auto Uniforms = m_Material->Uniforms();
    auto it = Uniforms.find("Modle");
    if (it != Uniforms.end()){
        m_Material->SetUniform("Modle", Transformation());
    }
}

auto GameObject::SetUp(std::vector<glm::vec3> InsPos) -> void
{
    m_InstancePos = std::move(InsPos);
    auto size = m_InstancePos.size();
    using VetexData = typename std::remove_cv_t<typename std::remove_reference_t<decltype(m_InstancePos)>::value_type>;

    auto currentVAO = Mesh::CurrentVAO();
    if(size > 1){
        GLuint VBO; // posible leak vbo think about it `TODO`
        glGenBuffers(1, &VBO);
        
        Expect(m_Mesh->VAO != 0, "VAO is 0");
        auto currentVBO = m_Mesh->CurrentVBO();

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size * sizeof(glm::vec3)), m_InstancePos.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, currentVBO);

        AttributeInfo positions{
            VetexData::length(),
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::vec3),
            0,
            1,
        };
    
        m_Mesh->setAttribute(3, positions); // dnt forget  3 is the index need better way
        glBindVertexArray(currentVAO);
    }
}



auto GameObject::transform() const -> Transform
{
    return m_Transform;
}

auto GameObject::Modle() const -> glm::mat4
{
    return Transformation();
}

auto GameObject::SetPosition(const glm::vec3 &pos) -> void
{
    m_Transform.position = pos;
}

auto GameObject::SetScale(const glm::vec3 &Scale) -> void
{
    m_Transform.scale = Scale;
}

auto GameObject::Rotate(const float &x, const float &y, const float &z) -> void
{
    m_Transform.rotation = {x, y, z};
}

auto GameObject::Transformation() const -> glm::mat4
{
    auto transformation = glm::mat4(1.0f);
    const auto& t = m_Transform;
    // Apply translation
    transformation = glm::translate(transformation, t.position);
    
    // Apply rotation (in radians)
    glm::quat rotation = glm::quat(glm::radians(glm::vec3(t.rotation.x, t.rotation.y, t.rotation.z)));
    transformation = glm::mat4_cast(rotation) * transformation;
    
    // Apply scaling
    transformation = glm::scale(transformation, t.scale);
    return transformation;
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
    Expect(m_Mesh->VAO != 0, "VAO is 0");
    glBindVertexArray(m_Mesh->VAO);
}

auto GameObject::Name() const -> std::string
{
    return m_Name;
}