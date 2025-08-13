#pragma once

#include <format>
#include <string>
#include <vector>

#include <core/gl.h>

class Shader
{
  public:
    friend struct std::formatter<Shader>;
    Shader();
    Shader(const std::string& filename);
    Shader(const std::string& Src, GLenum type);
    ~Shader() = default;

    Shader(const Shader&) = delete;
    auto operator=(const Shader&) -> Shader& = delete;

    Shader(Shader&& other) noexcept;
    auto operator=(Shader&& other) noexcept -> Shader&;

    bool operator==(const Shader& other);
  public:
    auto id() const                -> GLuint ;
    auto Type() const              -> GLenum ;
    auto TypeName() const          -> const char* ;

    auto SetSource(const std::string& src) const -> void;
    auto Compile()                           -> void;
    auto CheckCompileStatus() -> void;
    auto GetShaderInfo(GLenum what) const-> GLint; //what : GL_SHADER_TYPE, GL_DELETE_STATUS, GL_COMPILE_STATUS, GL_INFO_LOG_LENGTH, GL_SHADER_SOURCE_LENGTH.
    static auto PreProcess() -> std::string;
    
  private:
    GLuint m_Id;
    GLenum m_Type;

    FOR_TEST
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
    , obj.m_Id, obj.TypeName());
  }
};