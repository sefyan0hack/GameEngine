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
      auto Content() const           -> std::vector<GLchar>;

      static auto LoadFile(const char* filename) -> std::vector<GLchar>;
      static auto Compile(GLuint shader)             -> void;
      static auto LoadSource(const std::vector<GLchar>& src, GLuint shader) -> void;
      static auto checkShaderCompileStatus(const GLuint &shader) -> void;

  private:
      auto LoadSource()                   -> void;
    
  private:
      GLuint m_Id;
      GLenum m_Type;
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