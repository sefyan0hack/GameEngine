
#include "SkyBox.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include <core/res.hpp>

extern cmrc::embedded_filesystem embed_filesystem;

SkyBox::SkyBox()
  : m_VertShader(std::make_shared<Shader>(embed_filesystem.open("res/Shaders/skybox.vert"), GL_VERTEX_SHADER))
  , m_FragShader(std::make_shared<Shader>(embed_filesystem.open("res/Shaders/skybox.frag"), GL_FRAGMENT_SHADER))
  , m_Program(std::make_shared<ShaderProgram>(m_VertShader, m_FragShader))
  , m_Mesh(std::make_shared<Mesh>(Mesh::flip_faces(Mesh::CUBE)))
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

auto SkyBox::program() const -> std::shared_ptr<ShaderProgram>
{
  return m_Program;
}