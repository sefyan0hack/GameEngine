#include <core/Material.hpp>
#include <core/Log.hpp>
#include <core/Shader.hpp>
#include <core/Texture.hpp>
#include <core/Mesh.hpp>
#include <core/ResourceManager.hpp>
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

Material::Material(Material&& other) noexcept
    : m_Textuers(std::exchange(other.m_Textuers, {}))
{
}

Material::~Material()
{
}


auto Material::Bind(std::shared_ptr<ShaderProgram> program) const -> void
{

    for(const auto& [name, texture]: m_Textuers){
        texture->Bind();
        program->SetUniform(name, texture->TextureUnit());
    }
}


auto Material::texture(const std::string& name) const noexcept-> std::shared_ptr<Texture>
{
    return m_Textuers.at(name);
}

auto Material::SetTexture(const std::string &name, std::shared_ptr<Texture> texture) -> void
{
    m_Textuers[name] = texture;
}

auto Material::SetDiffuse(std::shared_ptr<Texture> texture) -> void
{
    m_Textuers["uDiffuseMap"] = texture;
}

auto Material::RemoveTexture(const std::string &name) -> bool
{
    return m_Textuers.erase(name);
}