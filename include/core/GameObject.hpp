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

    GameObject(const GameObject&) = default;
    auto operator=(const GameObject&) -> GameObject& = default;

    GameObject(GameObject&& other) noexcept;
    auto operator=(GameObject&& other) noexcept -> GameObject&;

    auto SetPosition(const glm::vec3 &pos)                   -> void ;
    auto SetScale(const glm::vec3 &Scale)                    -> void ;
    auto Rotate(float angle, glm::vec3 axis)                 -> void ;
    auto transform() const                                   -> Transform ;
    auto Model() const                                       -> glm::mat4 ;
    auto mesh() const                                        -> std::shared_ptr<Mesh> ;
    auto material() const                                    -> std::shared_ptr<Material> ;
    auto Bind() const                                        -> void;
    auto Name() const                                        -> std::string;

private:
    Transform m_Transform;
    std::shared_ptr<Material> m_Material;
    std::shared_ptr<Mesh> m_Mesh;
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
    , m_Texture(std::make_shared<TextureCubeMap>())
  {
  }

  SkyBox(std::shared_ptr<TextureCubeMap> texture)
    :  SkyBox()
  {
    m_Texture = texture;
  }

  SkyBox(const std::string& BasePathName)
    :  SkyBox()
  {
    m_Texture = std::make_shared<TextureCubeMap>(TextureCubeMap::base_to_6faces(BasePathName));
  }

  SkyBox(const SkyBox&) = delete;
  auto operator=(const SkyBox&) -> SkyBox& = delete;

  auto texture() const -> std::shared_ptr<TextureCubeMap>
  {
    return m_Texture;
  }

  auto mesh() const -> const Mesh&
  {
    return m_Mesh;
  }

  auto Program() const -> std::shared_ptr<ShaderProgram>
  {
    return m_Program;
  }

private:
  const std::shared_ptr<Shader> m_VertShader;
  const std::shared_ptr<Shader> m_FragShader;
  const std::shared_ptr<ShaderProgram> m_Program;
  Mesh m_Mesh;
  std::shared_ptr<TextureCubeMap> m_Texture;
};