#pragma once

#include <unordered_map>
#include <core/gl.h>
#include <core/fmts.hpp>
#include <format>

class Shader
{
    public:
        Shader(const char* name, GLenum type);
        ~Shader();
    public:
        auto Getid() const             -> GLuint ;
        auto GetType() const             -> GLenum ;
        auto GetTypeName() const             -> const char* ;
    private:
        auto Load(const char* name)    -> void ;
        auto LoadSource(const char* full_name_file)         -> void;
        auto Compile()                                      -> void;
        auto checkShaderCompileStatus(const GLuint &shader) -> void;
    
    private:
        GLuint id;
        GLenum Type;
        std::string Content;
};

// custom Mesh Format
template<>
struct std::formatter<Shader> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Shader& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "Shader: {{ id: {}, type: {} }}"
    , obj.Getid(), obj.GetTypeName());
  }
};

namespace {

    inline static std::unordered_map<GLenum, const char*> SHADERTYPES {
        { GL_VERTEX_SHADER, "GL_VERTEX_SHADER"},
        { GL_FRAGMENT_SHADER, "GL_FRAGMENT_SHADE"},
        { GL_COMPUTE_SHADER, "GL_COMPUTE_SHADER"},
        { GL_GEOMETRY_SHADER, "GL_GEOMETRY_SHADER"},
        { GL_TESS_CONTROL_SHADER, "GL_TESS_CONTROL_SHADER"},
        { GL_TESS_EVALUATION_SHADER, "GL_TESS_EVALUATION_SHADER"},
    };

}