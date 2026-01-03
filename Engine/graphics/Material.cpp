#include "Material.hpp"
#include "Texture.hpp"
#include "ShaderProgram.hpp"
#include "OpenGL.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <glm/matrix.hpp>

#define LAST_SLOT OpenGL::max_texture_units() - 1

Material::Material()
    : m_Diffuse(std::make_shared<Texture2D>())
{
}

Material::Material(std::shared_ptr<Texture> diffuse)
    : m_Diffuse(diffuse)
{
}


auto Material::bind(std::shared_ptr<ShaderProgram> program) -> void
{
    m_Diffuse->bind(0); // diffuse takes 0 slot
    program->set_uniform("uDiffuseMap", 0);

    // skybox().m_Texture->bind(LAST_SLOT); // skybox takes last slot && not needed to to bind more then ones
    program->set_uniform("uSkyboxMap", LAST_SLOT);
}


auto Material::diffuse() const noexcept-> std::shared_ptr<Texture>
{
    return m_Diffuse;
}

auto Material::set_diffuse(std::shared_ptr<Texture> texture) -> void
{
    m_Diffuse = texture;
}



auto Material::set_skybox(std::shared_ptr<Texture> texture) -> void
{
    skybox().set_texture(texture);
}

// in future this needed to be in renderer
auto Material::render_sky(const Camera& cam) -> void{

    auto& sk = skybox();
    gl::DepthFunc(GL_LEQUAL);

    sk.m_Program.use();
    sk.m_Program.set_uniform("View", glm::mat4(glm::mat3(cam.view())));
    sk.m_Program.set_uniform("Projection", cam.perspective());
    sk.m_Texture->bind(LAST_SLOT);
    sk.m_Program.set_uniform("uDiffuseMap", LAST_SLOT);

    gl::BindVertexArray(sk.m_DummyVAO);
    gl::DrawArrays(GL_TRIANGLES, 0, 36);

    gl::DepthFunc(GL_LESS);
}

auto Material::skybox() -> SkyBox&
{
    static SkyBox* skb;
    if(!skb) skb = new SkyBox();
    return *skb;
}