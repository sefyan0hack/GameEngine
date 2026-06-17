
#include "SkyBox.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "gl.hpp"

SkyBox::SkyBox()
  : m_Program(Shader::new_vertex("res/Shaders/skybox.vert"), Shader::new_fragment("res/Shaders/skybox.frag"))
  , m_Texture(std::make_shared<TextureCubeMap>(TextureCubeMap::base_to_6facesfiles("res/forest.jpg")))
  , VAO(0)
{
  gl::GenVertexArrays(1, &VAO);
}

SkyBox::SkyBox(std::shared_ptr<TextureCubeMap> texture)
  : SkyBox()
{
  set_texture(texture);
}

auto SkyBox::set_texture(std::shared_ptr<TextureCubeMap> texture) -> void
{
  m_Texture = texture;
}

auto SkyBox::render(const Camera& cam) -> void
{
  GLint old_depth{};
  gl::GetIntegerv(GL_DEPTH_FUNC, &old_depth);

  gl::DepthFunc(GL_LEQUAL);

  m_Program.use();
  m_Program.set_uniform("View", emath::mat4(emath::mat3(cam.view())));
  m_Program.set_uniform("Projection", cam.perspective());

  int skybox = 0;
  gl::ActiveTexture(GL_TEXTURE0 + skybox);
  m_Texture->bind();
  m_Program.set_uniform("uDiffuseMap", skybox);

  gl::BindVertexArray(VAO);
  gl::DrawArrays(GL_TRIANGLES, 0, 3);

  gl::DepthFunc(old_depth);
}
