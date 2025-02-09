#include <core/GameObject.hpp>
#include <core/Material.hpp>
#include <core/Log.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameObject::GameObject(glm::vec3 position, Material &matt, Mesh &mesh, std::string Name)
    : transform(Transform(position))
    , material(std::make_shared<Material>(std::move(matt)))
    , m_Mesh(std::make_shared<Mesh>(std::move(mesh)))
    , name(Name)
{   
    Transformation();
    UpMatrix();
    Log::Info("{}", *this);
    Count++;
}

GameObject::~GameObject()
{
}

auto GameObject::UpMatrix() -> void
{
    auto Uniforms = material->GetUniforms();
    auto it = Uniforms.find("Modle");
    if (it != Uniforms.end()){
        material->SetUniform("Modle", Transformation());
    }
}

auto GameObject::SetUp(std::vector<glm::vec3> InsPos) -> void
{
    InstancePos = std::move(InsPos);
    auto size = InstancePos.size();

    if(size > 1){
        GLuint VBO; // posible leak vbo think about it `TODO`
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size * sizeof(glm::vec3)), InstancePos.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(m_Mesh->attribs, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glVertexAttribDivisor(m_Mesh->attribs, 1); // Update once per instancedVertexArray(0);

        m_Mesh->attribs++;
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

auto GameObject::GetMesh() const -> std::shared_ptr<Mesh>
{
    return m_Mesh;
}
auto GameObject::GetInstancePos() const -> const std::vector<glm::vec3> &
{
    return InstancePos;
}

auto GameObject::GetMaterial() const -> std::shared_ptr<Material>
{
    return material;
}

auto GameObject::Bind() const -> void
{
    Log::Expect(m_Mesh->VAO != 0, "VAO is 0");
    glBindVertexArray(m_Mesh->VAO);
}