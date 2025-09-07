#include "Material.hpp"
#include "Texture.hpp"
#include "ShaderProgram.hpp"
#include <glm/glm.hpp>

Material::Material()
    : m_Textuers{ {"uDiffuseMap", std::make_shared<Texture2D>()} }
{
}

Material::Material(std::shared_ptr<Texture> diffuse)
    : m_Textuers{ {"uDiffuseMap", diffuse} }
{
}

Material::Material(const Material& other)
    : m_Textuers(other.m_Textuers)
{
}


auto Material::bind(std::shared_ptr<ShaderProgram> program) const -> void
{

    for(const auto& [name, texture]: m_Textuers){
        texture->bind();
        program->set_uniform(name, texture->texture_unit());
    }
}


auto Material::texture(const std::string& name) const noexcept-> std::shared_ptr<Texture>
{
    return m_Textuers.at(name);
}

auto Material::set_texture(const std::string &name, std::shared_ptr<Texture> texture) -> void
{
    m_Textuers[name] = texture;
}

auto Material::set_diffuse(std::shared_ptr<Texture> texture) -> void
{
    m_Textuers["uDiffuseMap"] = texture;
}

auto Material::remove_texture(const std::string &name) -> bool
{
    return m_Textuers.erase(name);
}