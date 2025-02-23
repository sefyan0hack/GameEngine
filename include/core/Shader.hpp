#pragma once

#include <unordered_map>
#include <core/gl.h>
#include <core/fmts.hpp>
#include <format>
#include <vector>

class Shader
{
    public:
        friend struct std::formatter<Shader>;
        Shader();
        Shader(const char* name, GLenum type);
        Shader(const Shader& other);
        Shader(Shader&& other);

        Shader& operator=(const Shader& other);

        bool operator==(const Shader& other);
        ~Shader();
    public:
        auto Getid() const                -> GLuint ;
        auto GetType() const              -> GLenum ;
        auto GetTypeName() const          -> const char* ;
        auto GetContent() const           -> std::vector<GLchar>;

        static auto LoadFile(const char* filename) -> std::vector<GLchar>;
        static auto Compile(GLuint shader)             -> void;
        static auto LoadSource(const std::vector<GLchar>& src, GLuint shader) -> void;
        static auto checkShaderCompileStatus(const GLuint &shader) -> void;

    private:
        auto LoadSource()                   -> void;
    
    private:
        GLuint id;
        GLenum Type;
        std::vector<GLchar> Content;
};

// custom Mesh Format
template<>
struct std::formatter<Shader> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Shader& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "{}: {{ id: {}, type: {} }}"
    , typeid(obj).name(), obj.id, obj.GetTypeName());
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