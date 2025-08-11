
#include <core/SkyBox.hpp>
#include <core/Shader.hpp>
#include <core/ShaderProgram.hpp>
#include <core/Mesh.hpp>
#include <core/Texture.hpp>

SkyBox::SkyBox()
  : m_VertShader(std::make_shared<Shader>(SHADER(skybox)".vert", GL_VERTEX_SHADER))
  , m_FragShader(std::make_shared<Shader>(SHADER(skybox)".frag", GL_FRAGMENT_SHADER))
  , m_Program(std::make_shared<ShaderProgram>(m_VertShader, m_FragShader))
  , m_Mesh(std::make_shared<Mesh>(Mesh::CUBE))
  , m_Texture(std::make_shared<TextureCubeMap>())
{
}

SkyBox::SkyBox(std::shared_ptr<TextureCubeMap> texture)
  :  SkyBox::SkyBox()
{
  m_Texture = texture;
}

SkyBox::SkyBox(const std::string& BasePathName)
  :  SkyBox::SkyBox()
{
  m_Texture = std::make_shared<TextureCubeMap>(TextureCubeMap::base_to_6faces(BasePathName));
}

auto SkyBox::texture() const -> std::shared_ptr<TextureCubeMap>
{
  return m_Texture;
}

auto SkyBox::mesh() const -> std::shared_ptr<Mesh>
{
  return m_Mesh;
}

auto SkyBox::Program() const -> std::shared_ptr<ShaderProgram>
{
  return m_Program;
}