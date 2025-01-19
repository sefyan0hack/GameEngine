#include <core/GameObject.hpp>
#include <core/Shader.hpp>
#include <string>
GameObject::GameObject(glm::vec3 position, const Shader &program, const Mesh &mesh)
: transform(Transform(position)), program(&program), m_Mesh(mesh)
{   
    Transformation();
    UpMatrix();
}

GameObject::~GameObject()
{
}

auto GameObject::UpMatrix() -> void
{
    program->SetUniform("Modle", Transformation());
}

auto GameObject::SetUp(std::vector<glm::vec3> InsPos) -> void
{
    InstancePos = std::move(InsPos);
    auto size = InstancePos.size();
    m_Mesh.setupMesh();
    if(size > 1){
        GLuint UBO;
        glGenBuffers(1, &UBO);
        glBindBuffer(GL_ARRAY_BUFFER, UBO);
        glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec3), InstancePos.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glVertexAttribDivisor(3, 1);
        glBindVertexArray(0);
    }
}

auto GameObject::Render() -> void
{
    auto sizeIns = InstancePos.size();

    if(sizeIns > 1){
        m_Mesh.Draw(*program, InstancePos.size());
    }else{
        m_Mesh.Draw(*program, 1);
    }
}

Transform GameObject::GetTransform() const
{
    return transform;
}

glm::mat4 GameObject::GetModleMatrix() const
{
    return Transformation();
}

auto GameObject::SetPosition(const glm::vec3 &pos) -> void
{
    transform.position = pos;
}

auto GameObject::SetScale(const glm::vec3 &Scale) -> void
{
    transform.scale = Scale;
}

auto GameObject::Rotate(const float &x, const float &y, const float &z) -> void
{
    transform.rotation = {x, y, z};
}

auto GameObject::Transformation() const -> glm::mat4
{
    static auto transformation = glm::mat4(1.0f);
    transformation = glm::mat4(1.0f);
    auto t = transform;
    // Apply translation
    transformation = glm::translate(transformation, t.position);
    
    // Apply rotation (in radians)
    glm::quat rotation = glm::quat(glm::radians(glm::vec3(t.rotation.x, t.rotation.y, t.rotation.z)));
    transformation = glm::mat4_cast(rotation) * transformation;
    
    // Apply scaling
    transformation = glm::scale(transformation, t.scale);
    return transformation;
}
