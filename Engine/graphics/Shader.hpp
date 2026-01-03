#pragma once

#include <format>
#include <string>
#include <vector>
#include <engine_export.h>

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

    auto set_source(const std::string& src) const -> void;
    auto compile()                           -> void;
    auto check_compile_status() -> void;
    auto get_shader_info(GLenum what) const-> GLint; //what : GL_SHADER_TYPE, GL_DELETE_STATUS, GL_COMPILE_STATUS, GL_INFO_LOG_LENGTH, GL_SHADER_SOURCE_LENGTH.
    static auto pre_process() -> std::string;
    static auto new_vertex(either<std::string, cmrc::file> auto const & vert) -> std::shared_ptr<Shader>
    {
      return std::make_shared<Shader>(vert, GL_VERTEX_SHADER);
    }

    static auto new_fragment(either<std::string, cmrc::file> auto const & frag) -> std::shared_ptr<Shader>
    {
      return std::make_shared<Shader>(frag, GL_FRAGMENT_SHADER);
    }

  private:
    GLuint m_Id;
    GLenum m_Type;

    
};

// custom Shader Format
template<>
struct std::formatter<Shader> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Shader& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "id": {}, "type": "{}" }})"
    , obj.m_Id, obj.type_name());
  }
};