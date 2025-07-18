#pragma once
#include <glm/glm.hpp>
#include <core/Mesh.hpp>
#include <core/Transform.hpp>
#include <core/Material.hpp>
#include <core/fmts.hpp>
#include <core/AutoRelease.hpp>
#include <core/Camera.hpp>
#include <core/OpenGL.hpp>


class GameObject {
public:
    friend struct std::formatter<GameObject>;
    GameObject(glm::vec3 position, Material& matt, Mesh& mesh, std::string Name = std::format("Obj{}", Count));
    GameObject(Transform transform, Material& matt, Mesh& mesh, std::string Name = std::format("Obj{}", Count));
    ~GameObject();

    auto UpMatrix()                                             -> void ;
    auto SetUp(std::vector<glm::vec3> InsPos)                   -> void ;
    auto SetPosition(const glm::vec3 &pos)                      -> void ;
    auto SetScale(const glm::vec3 &Scale)                       -> void ;
    auto Rotate(const float &x, const float &y, const float &z) -> void ;
    auto transform() const                                   -> Transform ;
    auto Model() const                                 -> glm::mat4 ;
    auto mesh() const                                        -> std::shared_ptr<Mesh> ;
    auto material() const                                    -> std::shared_ptr<Material> ;
    auto InstancePos() const                                 -> const std::vector<glm::vec3>&;
    auto Bind() const                                           -> void;
    auto Name() const                                           -> std::string;

private:
    auto Transformation() const                                 -> glm::mat4 ;
private:
    Transform m_Transform;
    std::shared_ptr<Material> m_Material;
    std::shared_ptr<Mesh> m_Mesh;
    std::vector<glm::vec3> m_InstancePos;
    std::string m_Name;
    inline static std::size_t Count = 1;

    FOR_TEST
};

// custom GameObject Format
template<>
struct std::formatter<GameObject> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const GameObject& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{"name": "{}", "transform": {}, "material": {}, "mesh": {} }})"
    , obj.m_Name, obj.m_Transform, *obj.m_Material, *obj.m_Mesh.get());
  }
};

class SkyBox
{
public:
  SkyBox(glm::vec3 position, Material& matt, Mesh& mesh)
    : m_skybox(GameObject(position, matt, mesh))
  {
  }
  operator GameObject() const {
    return m_skybox;
  }
  auto render(const Camera& camera) -> void
  {
    auto mat = m_skybox.material();
    auto mesh = m_skybox.mesh();
    auto TextureUnit = mat->texture()->TextureUnit();
    
    gl::DepthFunc(GL_LEQUAL);
    mat->Use();
    mat->SetUniform("View", glm::mat4(glm::mat3(camera.View())));
    mat->SetUniform("Projection", camera.Perspective());
    mat->SetUniform("uDiffuseMap", TextureUnit);
    mesh->Bind();
    gl::DrawArrays(GL_TRIANGLES, 0, mesh->VextexSize());
    mesh->DisableAttribs();
    gl::DepthFunc(GL_LESS);
  }
  auto gameObject() const -> const GameObject&
  {
    return m_skybox;
  }

private:
    const GameObject m_skybox;
};