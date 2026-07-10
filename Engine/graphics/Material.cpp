#include "Material.hpp"

#include "Texture.hpp"

Material::Material()
    : m_Diffuse(Texture::texture_2d(nullptr))
{
}

Material::Material(std::shared_ptr<Texture> diffuse)
    : m_Diffuse(diffuse)
{
}

auto Material::diffuse() const noexcept-> std::shared_ptr<Texture>
{
    return m_Diffuse;
}

auto Material::set_diffuse(std::shared_ptr<Texture> texture) -> void
{
    m_Diffuse = texture;
}