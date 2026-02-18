#pragma once

#include <format>
#include <string>
#include <vector>
#include <memory>
#include <engine_export.h>
#include <span>
#include "gl.hpp"

#include <cmrc/cmrc.hpp>

class ENGINE_EXPORT Shader
{
  public:
    friend struct std::formatter<Shader>;
    Shader();
    Shader(const std::string& filename);
    Shader(std::string Src, GLenum type);
    Shader(const cmrc::file& Src, GLenum type);

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
    auto check_compile_status() -> void;
    auto get_shader_info(GLenum what) const-> GLint; //what : GL_SHADER_TYPE, GL_DELETE_STATUS, GL_COMPILE_STATUS, GL_INFO_LOG_LENGTH, GL_SHADER_SOURCE_LENGTH.

    static auto new_vertex(const std::string& vert) -> std::shared_ptr<Shader>;
    static auto new_vertex(const cmrc::file& vert) -> std::shared_ptr<Shader>;
  
    static auto new_fragment(const cmrc::file& frag) -> std::shared_ptr<Shader>;
    static auto new_fragment(const std::string& frag) -> std::shared_ptr<Shader>;
  
  private:
    GLuint m_Id;
    GLenum m_Type;

    static std::string glsl_header;
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