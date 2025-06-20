#pragma once

#include <core/OpenGL.hpp>
#include <core/fmts.hpp>

class Shader
{
  public:
      friend struct std::formatter<Shader>;
      Shader();
      Shader(const char* name, GLenum type);
      Shader(const Shader& other);
      Shader(Shader&& other) noexcept;
      auto operator=(const Shader& other) -> Shader&;

      bool operator==(const Shader& other);
      ~Shader();
  public:
      auto id() const                -> GLuint ;
      auto Type() const              -> GLenum ;
      auto TypeName() const          -> const char* ;
      auto File() const              -> std::string;
      auto Content() const           -> std::vector<GLchar>;
      
      
      static auto LoadFile(const char* filename) -> std::vector<GLchar>;
      static auto Compile(GLuint shader)             -> void;
      static auto LoadSource(const std::vector<GLchar>& src, GLuint shader) -> void;
      static auto checkShaderCompileStatus(const Shader &shader) -> void;
      static auto GetShaderInfo(GLuint id, GLenum what) -> std::optional<GLint>; //what : GL_SHADER_TYPE, GL_DELETE_STATUS, GL_COMPILE_STATUS, GL_INFO_LOG_LENGTH, GL_SHADER_SOURCE_LENGTH.

  private:
      auto LoadSource()                   -> void;
    
  private:
      GLuint m_Id;
      GLenum m_Type;
      std::string m_File;
      std::vector<GLchar> m_Content;
      FOR_TEST
};

// custom Mesh Format
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