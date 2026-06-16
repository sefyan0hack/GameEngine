#include "Material.hpp"
#include "Texture.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <emath/mat3.hpp>
#include <emath/mat4.hpp>

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