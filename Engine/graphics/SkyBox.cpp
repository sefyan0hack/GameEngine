
#include "SkyBox.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"

constexpr auto vert = R"GLSL(  
  out vec3 Uv;

  uniform mat4 Projection;
  uniform mat4 View;

  void main() {
    vec3 corners[8] = vec3[](
        vec3(-1.0, -1.0,  1.0), vec3( 1.0, -1.0,  1.0), vec3( 1.0,  1.0,  1.0), vec3(-1.0,  1.0,  1.0), // Front face z+
        vec3(-1.0, -1.0, -1.0), vec3( 1.0, -1.0, -1.0), vec3( 1.0,  1.0, -1.0), vec3(-1.0,  1.0, -1.0)  // Back face z-
    );

    int indices[36] = int[](
        // Right
        1, 2, 6, 6, 5, 1,
        // Left
        0, 4, 7, 7, 3, 0,
        // Top
        3, 7, 6, 6, 2, 3,
        // Bottom
        0, 1, 5, 5, 4, 0,
        // Back
        4, 5, 6, 6, 7, 4,
        // Front
        0, 3, 2, 2, 1, 0
    );

    vec3 aPos = corners[indices[gl_VertexID]];

    Uv = aPos;
    vec4 pos = Projection * View * vec4(aPos, 1.0);
    
    // Force depth to 1.0
    // allowing the skybox to be drawn last for performance.
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
  : m_Program(Shader::new_vertex(vert), Shader::new_fragment(frag))
  , m_Texture(std::make_shared<TextureCubeMap>())
{ gl::GenVertexArrays(1, &m_DummyVAO); }

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