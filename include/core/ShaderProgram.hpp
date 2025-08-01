#pragma once
#include <core/gl.h>
#include <core/Shader.hpp>


class ShaderProgram {
public:
    using GLSLVar = std::tuple<GLuint, GLenum, GLsizei>;

    friend struct std::formatter<ShaderProgram>;
    ShaderProgram(std::shared_ptr<Shader> vertex, std::shared_ptr<Shader> fragment);
    ~ShaderProgram();

    // ShaderProgram(const ShaderProgram& other);
    ShaderProgram(ShaderProgram&& other) noexcept;
    
    auto id() const noexcept -> GLuint ;
    auto Use() const -> void ;
    auto UniformCount() const                  -> GLint ;
    auto AttribsCount() const                  -> GLint ;
    auto UniformLocation(const char*) const -> GLuint;
    auto AttribLocation(const char*) const -> GLuint;
    auto Uniforms() const noexcept -> const std::map<std::string, GLSLVar>&;
    auto Attribs() const noexcept -> const std::map<std::string, GLSLVar>&;
    static auto Current_Program() -> GLuint;
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
    static auto checkProgramLinkStatus(const ShaderProgram& aterial) -> void;
    auto Link() const -> void;
    auto UniformLocation_Prv(const char* name) const -> GLuint;
    auto AttribLocation_Prv(const char* name) const -> GLuint;
    auto DumpUniforms()                   -> void ;
    auto DumpAttribs()                    -> void ;

private:
    GLuint m_Id;
    std::vector<std::shared_ptr<Shader>> m_Shaders;
    std::map<std::string, GLSLVar> m_Attribs;
    std::map<std::string, GLSLVar> m_Uniforms;

    FOR_TEST

};

// custom ShaderProgram::GLSLVar Format
template<>
struct std::formatter<ShaderProgram::GLSLVar> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const ShaderProgram::GLSLVar& obj, std::format_context& context) const {
    auto [loc, type, size] = obj;
    return std::format_to(context.out(),
    R"({{ "loc": {}, "type": {}, "size": {} }})"
    , loc, ShaderProgram::GLSL_Type_to_string(type), size);
  }
};

// custom ShaderProgram Format
template<>
struct std::formatter<ShaderProgram> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const ShaderProgram& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "id": {}, "attribs": {}, "uniforms": {} }})"
    , obj.m_Id, MapWrapper{obj.m_Attribs}, MapWrapper{obj.m_Uniforms});
  }
};
