#include "Material.hpp"
#include "Texture.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <glm/matrix.hpp>

constexpr int32_t Diffuse_SLOT = 0;
constexpr int32_t SkyBox_SLOT = 1;

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
    gl::ActiveTexture(GL_TEXTURE0 + Diffuse_SLOT);
    m_Diffuse->bind();
    program->set_uniform("uDiffuseMap", Diffuse_SLOT);

    gl::ActiveTexture(GL_TEXTURE0 + SkyBox_SLOT);
    program->set_uniform("uSkyboxMap", SkyBox_SLOT);
}

auto Material::diffuse() const noexcept-> std::shared_ptr<Texture>
{
    return m_Diffuse;
}

auto Material::set_diffuse(std::shared_ptr<Texture> texture) -> void
{
    m_Diffuse = texture;
}

auto Material::set_skybox(std::shared_ptr<TextureCubeMap> texture) -> void
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
    gl::ActiveTexture(GL_TEXTURE0 + SkyBox_SLOT);
    sk.m_Texture->bind();
    sk.m_Program.set_uniform("uDiffuseMap", SkyBox_SLOT);

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