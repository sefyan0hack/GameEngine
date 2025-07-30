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
    GameObject(glm::vec3 position, std::shared_ptr<Material> matt, std::shared_ptr<Mesh> mesh, std::string Name = std::format("Obj{}", Count));
    GameObject(Transform transform, std::shared_ptr<Material> matt, std::shared_ptr<Mesh> mesh, std::string Name = std::format("Obj{}", Count));
    ~GameObject();

    auto SetUp(std::vector<glm::vec3> InsPos)                -> void ;
    auto SetPosition(const glm::vec3 &pos)                   -> void ;
    auto SetScale(const glm::vec3 &Scale)                    -> void ;
    auto Rotate(float angle, glm::vec3 axis)                 -> void ;
    auto transform() const                                   -> Transform ;
    auto Model() const                                       -> glm::mat4 ;
    auto mesh() const                                        -> std::shared_ptr<Mesh> ;
    auto material() const                                    -> std::shared_ptr<Material> ;
    auto InstancePos() const                                 -> const std::vector<glm::vec3>&;
    auto Bind() const                                        -> void;
    auto Name() const                                        -> std::string;

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
    R"({{"name": "{}", "transform": {}, "mesh": {} }})"
    , obj.m_Name, obj.m_Transform, *obj.m_Mesh.get());
  }
};

class SkyBox
{
public:
  
  SkyBox()
    : m_VertShader(std::make_shared<Shader>(SHADER(skybox)".vert", GL_VERTEX_SHADER))
    , m_FragShader(std::make_shared<Shader>(SHADER(skybox)".frag", GL_FRAGMENT_SHADER))
    , m_Program(std::make_shared<ShaderProgram>(m_VertShader, m_FragShader))
    , m_Mesh(Mesh::CUBE)
    , m_Material()
  {
    m_Material.SetTexture("uDiffuseMap", std::make_shared<TextureCubeMap>());
  }

  SkyBox(const std::string& BasePathName)
    :  SkyBox()
  {
    m_Material.SetTexture("uDiffuseMap", std::make_shared<TextureCubeMap>(
      TextureCubeMap::base_to_6faces(BasePathName))
    );
  }

  SkyBox(const SkyBox&) = delete;
  auto operator=(const SkyBox&) -> SkyBox& = delete;

  auto render(const Camera& camera) -> void
  {
    gl::DepthFunc(GL_LEQUAL);
    m_Program->Use();
    m_Program->SetUniform("View", glm::mat4(glm::mat3(camera.View())));
    m_Program->SetUniform("Projection", camera.Perspective());
    m_Mesh.Bind();
    m_Material.Bind(m_Program);
    gl::DrawArrays(GL_TRIANGLES, 0, m_Mesh.VextexSize());
    gl::DepthFunc(GL_LESS);
  }

  auto texture() -> std::shared_ptr<Texture>
  {
    return m_Material.texture("uDiffuseMap");
  }

private:
  const std::shared_ptr<Shader> m_VertShader;
  const std::shared_ptr<Shader> m_FragShader;
  const std::shared_ptr<ShaderProgram> m_Program;
  Mesh m_Mesh;
  Material m_Material;
};