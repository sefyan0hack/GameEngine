#pragma once

#include "gl.hpp"
#include <engine_export.h>

#include <format>
#include <string>
#include <vector>
#include <memory>
#include <span>


class ENGINE_EXPORT Shader
{
  public:
    friend struct std::formatter<Shader>;

    Shader(const char* shader, GLenum type);
    Shader(const std::string& shader, GLenum type);

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;
  
    ~Shader();

  public:
    auto id() const                -> GLuint ;
    auto type() const              -> GLenum ;
    auto type_name() const          -> const char* ;

    auto set_sources(const std::span<const char* const> srcs) const -> void;
    auto compile()                           -> void;
    auto check_compile_status() -> std::string;
    auto get_shader_info(GLenum what) const-> GLint; //what : GL_SHADER_TYPE, GL_DELETE_STATUS, GL_COMPILE_STATUS, GL_INFO_LOG_LENGTH, GL_SHADER_SOURCE_LENGTH.

    static auto new_vertex(const std::string& vert) -> std::shared_ptr<Shader>;
    static auto new_fragment(const std::string& frag) -> std::shared_ptr<Shader>;

    static auto glsl_header() -> std::string;
    static auto glsl_lib() -> std::string;

  private:
    GLuint m_Id;
    GLenum m_Type;
};

#ifdef __cpp_lib_formatters
// custom Shader Format
template<>
struct std::formatter<Shader> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Shader& obj, auto& context) const {
    return std::format_to(context.out(),
    R"({{ "id": {}, "type": "{}" }})"
    , obj.m_Id, obj.type_name());
  }
};
#endif