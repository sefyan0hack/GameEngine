#include <core/Material.hpp>
#include <core/Log.hpp>
#include <core/Shader.hpp>
#include <core/Texture.hpp>
#include <core/Mesh.hpp>
#include <core/ResourceManager.hpp>
#include <glm/glm.hpp>


Material::Material(std::shared_ptr<ShaderProgram> program)
    : m_ShaderProgram(program)
    , m_Textuers{ {"uDiffuseMap", std::make_shared<Texture2D>()} }
{
}


Material::Material(std::shared_ptr<Shader> vertex, std::shared_ptr<Shader> fragment)
    : m_ShaderProgram(std::make_shared<ShaderProgram>(vertex, fragment))
    , m_Textuers{ {"uDiffuseMap", std::make_shared<Texture2D>()} }
{
}

Material::Material(const Material& other)
    : m_ShaderProgram(other.m_ShaderProgram)
    , m_Textuers(other.m_Textuers)
{
}

Material::Material(Material&& other) noexcept
    : m_ShaderProgram(std::exchange(other.m_ShaderProgram, nullptr))
    , m_Textuers(std::exchange(other.m_Textuers, {}))
{
}

Material::~Material()
{
}


auto Material::Use() const -> void
{
    m_ShaderProgram->Use();

    for(const auto& [name, texture]: m_Textuers){
        texture->Bind();
        m_ShaderProgram->SetUniform(name, texture->TextureUnit());
    }
}

auto Material::Program() -> std::shared_ptr<ShaderProgram>
{
    return m_ShaderProgram;
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
