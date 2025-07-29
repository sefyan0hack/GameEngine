#pragma once
#include <core/OpenGL.hpp>
#include <core/fmts.hpp>
#include <core/ShaderProgram.hpp>
#include <core/Texture.hpp>
#include <core/Mesh.hpp>

class Material
{
public:
                              //loc    type    size
    using GLSLVar = std::tuple<GLuint, GLenum, GLsizei>;

    friend struct std::formatter<Material>;
    Material(std::shared_ptr<ShaderProgram> program);
    Material(std::shared_ptr<Shader> vertex, std::shared_ptr<Shader> fragment);
    // Material(std::initializer_list<Shader> shaders);

    Material(const Material& other);
    Material(Material&& other) noexcept;
    ~Material();

    auto Use() const -> void;
    auto Program() -> std::shared_ptr<ShaderProgram>;

    auto texture(const std::string& name) const noexcept -> std::shared_ptr<Texture>;
    auto SetTexture(const std::string &name, std::shared_ptr<Texture> texture) -> void;
    auto SetDiffuse(std::shared_ptr<Texture> texture) -> void;
    // auto Shaders() const noexcept -> const std::vector<std::shared_ptr<Shader>>&;
    auto SetUniform(const std::string &name, auto value) const -> void
    {
      m_ShaderProgram->SetUniform(name, value);
    }

private:
    std::shared_ptr<ShaderProgram> m_ShaderProgram;
    std::map<std::string, std::shared_ptr<Texture>> m_Textuers;

    FOR_TEST
};


// custom Material Format
template<>
struct std::formatter<Material> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Material& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "Program": {} }})"
    , *obj.m_ShaderProgram);
  }
};