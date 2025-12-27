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


auto Material::bind(std::shared_ptr<ShaderProgram> program) const -> void
{
    uint8_t slot = 0;
    for(const auto& [name, texture]: m_Textuers){
        texture->bind(slot);
        program->set_uniform(name, slot);
        slot++;
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