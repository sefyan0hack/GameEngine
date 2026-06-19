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
    int diffuse = 0;

    gl::ActiveTexture(GL_TEXTURE0 + diffuse);

    m_Diffuse->bind();
    program->set_uniform("uDiffuseMap", diffuse);
}

auto Material::diffuse() const noexcept-> std::shared_ptr<Texture>
{
    return m_Diffuse;
}

auto Material::set_diffuse(std::shared_ptr<Texture> texture) -> void
{
    m_Diffuse = texture;
}