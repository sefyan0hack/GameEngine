#pragma once
#include <tuple>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <format>

#include "gl.hpp"
#include <glm/glm.hpp>
#include <core/fmts.hpp>
#include <engine_export.h>


class ENGINE_EXPORT ShaderProgram {
public:
    using GlslType = std::tuple<GLuint, GLenum, GLsizei>;

    friend struct std::formatter<ShaderProgram>;
    ShaderProgram(std::shared_ptr<class Shader> vertex, std::shared_ptr<class Shader> fragment);
    ~ShaderProgram();

    // ShaderProgram(const ShaderProgram& other);
    ShaderProgram(ShaderProgram&& other) noexcept;
    
    auto id() const noexcept -> GLuint ;
    auto use() const -> void ;
    auto uniform_count() const                  -> GLint ;
    auto attribs_count() const                  -> GLint ;
    auto uniform_location(const char*) const -> GLuint;
    auto attrib_location(const char*) const -> GLuint;
    auto uniforms() const noexcept -> const std::map<std::string, GlslType>&;
    auto attribs() const noexcept -> const std::map<std::string, GlslType>&;
    static auto current_program() -> GLuint;
    static auto glsl_type_to_string(GLenum type) -> const char*;

    
    auto set_uniform(const std::string &name, const GLuint &value) const -> void;
    auto set_uniform(const std::string &name, const GLfloat &value) const -> void;
    auto set_uniform(const std::string &name, const GLint &value) const -> void;
    auto set_uniform(const std::string &name, const glm::vec2 &value) const -> void;
    auto set_uniform(const std::string &name, const glm::vec3 &value) const -> void;
    auto set_uniform(const std::string &name, const glm::vec4 &value) const -> void;
    auto set_uniform(const std::string &name, const glm::mat2 &value) const -> void;
    auto set_uniform(const std::string &name, const glm::mat3 &value) const -> void;
    auto set_uniform(const std::string &name, const glm::mat4 &value) const -> void;

private:
    auto get_program_info(GLenum what) const -> GLint;
    auto link() const -> void;
    auto uniform_location_prv(const char* name) const -> GLuint;
    auto attrib_location_prv(const char* name) const -> GLuint;
    auto dump_uniforms()                   -> void ;
    auto dump_attribs()                    -> void ;

private:
    GLuint m_Id;
    std::vector<std::shared_ptr<class Shader>> m_Shaders;
    std::map<std::string, GlslType> m_Attribs;
    std::map<std::string, GlslType> m_Uniforms;

    

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
    loc, ShaderProgram::glsl_type_to_string(type), size);
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
