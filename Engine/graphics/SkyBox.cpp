
#include "SkyBox.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"

constexpr auto vert = R"GLSL(
  layout (location = 0) in vec3 aPos;

  out vec3 Uv;

  uniform mat4 Projection;
  uniform mat4 View;

  void main() {
    Uv = aPos;
    vec4 pos = Projection * View * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
  }
)GLSL";


constexpr auto frag = R"GLSL(
  out vec4 FragColor;
  in vec3 Uv;

  uniform samplerCube uDiffuseMap;

  void main() {
    FragColor = texture(uDiffuseMap, Uv);
  }
)GLSL";


SkyBox::SkyBox()
  : m_VertShader(vert, GL_VERTEX_SHADER)
  , m_FragShader(frag, GL_FRAGMENT_SHADER)
  , m_Program(std::make_shared<Shader>(m_VertShader), std::make_shared<Shader>(m_FragShader))
  , m_Mesh(Mesh::flip_faces(Mesh::CUBE))
  , m_Texture(std::make_shared<TextureCubeMap>())
{}

SkyBox::SkyBox(std::shared_ptr<TextureCubeMap> texture)
  : SkyBox()
{
  set_texture(texture);
}

SkyBox::SkyBox(const std::string& BasePathName)
  : SkyBox()
{
  set_texture(BasePathName);
}

auto SkyBox::set_texture(std::shared_ptr<TextureCubeMap> texture) -> void
{
  m_Texture = texture;
}

auto SkyBox::set_texture(const std::string& BasePathName) -> void
{
  m_Texture = std::make_shared<TextureCubeMap>(TextureCubeMap::base_to_6faces(BasePathName));
}