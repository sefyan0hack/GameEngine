#include <core/GameObject.hpp>
#include <core/Shader.hpp>
#include <string>
GameObject::GameObject(glm::vec3 position, const Shader &program, const Mesh &mesh)
: transform(Transform(position)), program(&program), m_Mesh(mesh), InstanceCount(1), InstancePos(nullptr)
{   
    Transformation();
    UpMatrix();
}

GameObject::~GameObject()
{
    if (this->InstancePos){
        delete [] this->InstancePos;
    }
}

auto GameObject::UpMatrix() -> void
{
    program->SetUniform("Modle", Transformation());
}

auto GameObject::SetUp(const std::vector<glm::vec3> &InsPos) -> void
{
    this->InstanceCount = InsPos.size();
    this->InstancePos = InsPos.data();
    m_Mesh.setupMesh();
    if(this->InstanceCount > 1){
        GLuint UBO;
        glGenBuffers(1, &UBO);
        glBindBuffer(GL_ARRAY_BUFFER, UBO);
        glBufferData(GL_ARRAY_BUFFER, this->InstanceCount * sizeof(glm::vec3), InstancePos, GL_STATIC_DRAW);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glVertexAttribDivisor(3, 1);
        glBindVertexArray(0);
    }
}
auto GameObject::SetUp(glm::vec3 * InsPos, size_t size) -> void
{
    this->InstanceCount = size;
    this->InstancePos = InsPos;
    m_Mesh.setupMesh();
    if(this->InstanceCount > 1){
        GLuint UBO;
        glGenBuffers(1, &UBO);
        glBindBuffer(GL_ARRAY_BUFFER, UBO);
        glBufferData(GL_ARRAY_BUFFER, this->InstanceCount * sizeof(glm::vec3), &this->InstancePos[0][0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glVertexAttribDivisor(3, 1);
        glBindVertexArray(0);
    }
}
auto GameObject::Render() -> void
{
    m_Mesh.Draw(*program, this->InstanceCount);
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
