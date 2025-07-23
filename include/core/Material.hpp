#pragma once
#include <core/OpenGL.hpp>
#include <core/fmts.hpp>
#include <core/Shader.hpp>
#include <core/Texture.hpp>
#include <core/Mesh.hpp>

class Material
{
public:
                              //loc    type    size
    using GLSLVar = std::tuple<GLuint, GLenum, GLsizei>;

    friend struct std::formatter<Material>;
    Material(const Shader& vertex, const Shader& fragment);
    // Material(std::initializer_list<Shader> shaders);

    Material(const Material& other);
    Material(Material&& other) noexcept;
    ~Material();

    auto id() const noexcept -> GLuint ;
    auto Use() const -> void ;
    auto UniformCount() const                  -> GLint ;
    auto AttribsCount() const                  -> GLint ;
    auto UniformLocation(const char*) const -> GLuint;
    auto AttribLocation(const char*) const -> GLuint;
    auto Uniforms() const noexcept -> const std::map<std::string, GLSLVar>&;
    auto Attribs() const noexcept -> const std::map<std::string, GLuint>&;
    static auto Current_Program() -> GLuint;
    auto texture() const noexcept -> std::shared_ptr<Texture>;
    auto SetTexture(const std::string &name) -> void;
    auto Shaders() const noexcept -> const std::vector<Shader>&;
    static auto GLSL_Type_to_string(GLenum type) -> const char*;

    auto SetUniform(const std::string &name, const GLuint &value) const -> void;
    auto SetUniform(const std::string &name, const GLfloat &value) const -> void;
    auto SetUniform(const std::string &name, const GLint &value) const -> void;
    auto SetUniform(const std::string &name, const glm::vec2 &value) const -> void;
    auto SetUniform(const std::string &name, const glm::vec3 &value) const -> void;
    auto SetUniform(const std::string &name, const glm::vec4 &value) const -> void;
    auto SetUniform(const std::string &name, const glm::mat2 &value) const -> void;
    auto SetUniform(const std::string &name, const glm::mat3 &value) const -> void;
    auto SetUniform(const std::string &name, const glm::mat4 &value) const -> void;

private:
    static auto GetProgramInfo(GLint id, GLenum what)           -> std::optional<GLint>;
    static auto checkProgramLinkStatus(const Material& aterial) -> void;
    auto Link() const -> void;
    auto UniformLocation_Prv(const char* name) const -> GLuint;
    auto AttribLocation_Prv(const char* name) const -> GLuint;
    auto DumpUniforms()                   -> void ;
    auto DumpAttribs()                    -> void ;

private:
    GLuint m_Id;
    std::vector<Shader> m_Shaders;
    std::map<std::string, GLuint> m_Attribs;
    std::map<std::string, GLSLVar> m_Uniforms;
    std::shared_ptr<Texture> m_Albedo;

    FOR_TEST
};

// custom Material::GLSLVar Format
template<>
struct std::formatter<Material::GLSLVar> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Material::GLSLVar& obj, std::format_context& context) const {
    auto [loc, type, size] = obj;
    return std::format_to(context.out(),
    R"({{ "loc": {}, "type": {}, "size": {} }})"
    , loc, Material::GLSL_Type_to_string(type), size);
  }
};
// custom Material Format
template<>
struct std::formatter<Material> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Material& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "id": {}, "attribs": {}, "uniforms": {} }})"
    , obj.m_Id, MapWrapper{obj.m_Attribs}, MapWrapper{obj.m_Uniforms});
  }
};