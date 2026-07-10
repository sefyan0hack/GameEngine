#include "Material.hpp"

#include "Texture.hpp"

Material::Material()
    : m_Diffuse(std::make_shared<Texture2D>())
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