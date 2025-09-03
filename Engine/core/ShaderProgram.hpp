#pragma once
#include <tuple>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <format>

#include "gl.h"
#include <glm/glm.hpp>
#include "fmts.hpp"


class ShaderProgram {
public:
    using GlslType = std::tuple<GLuint, GLenum, GLsizei>;

    friend struct std::formatter<ShaderProgram>;
    ShaderProgram(std::shared_ptr<class Shader> vertex, std::shared_ptr<class Shader> fragment);
    ~ShaderProgram();

    // ShaderProgram(const ShaderProgram& other);
    ShaderProgram(ShaderProgram&& other) noexcept;
    
    auto id() const noexcept -> GLuint ;
    auto Use() const -> void ;
    auto UniformCount() const                  -> GLint ;
    auto AttribsCount() const                  -> GLint ;
    auto UniformLocation(const char*) const -> GLuint;
    auto AttribLocation(const char*) const -> GLuint;
    auto Uniforms() const noexcept -> const std::map<std::string, GlslType>&;
    auto Attribs() const noexcept -> const std::map<std::string, GlslType>&;
    static auto Current_Program() -> GLuint;
    static auto GlslType_to_string(GLenum type) -> const char*;

    
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
    auto GetProgramInfo(GLenum what) const -> GLint;
    auto Link() const -> void;
    auto UniformLocation_Prv(const char* name) const -> GLuint;
    auto AttribLocation_Prv(const char* name) const -> GLuint;
    auto DumpUniforms()                   -> void ;
    auto DumpAttribs()                    -> void ;

private:
    GLuint m_Id;
    std::vector<std::shared_ptr<class Shader>> m_Shaders;
    std::map<std::string, GlslType> m_Attribs;
    std::map<std::string, GlslType> m_Uniforms;

    FOR_TEST

};

// custom ShaderProgram::GlslType Format
template<>
struct std::formatter<ShaderProgram::GlslType> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const ShaderProgram::GlslType& obj, std::format_context& context) const {
    auto [loc, type, size] = obj;
    return std::format_to(context.out(),
    R"({{ "loc": {}, "type": {}, "size": {} }})",
    loc, ShaderProgram::GlslType_to_string(type), size);
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
    R"({{ "id": {}, "attribs": {}, "uniforms": {} }})",
    obj.m_Id, MapWrapper{obj.m_Attribs}, MapWrapper{obj.m_Uniforms});
  }
};
