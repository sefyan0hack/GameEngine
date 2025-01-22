#pragma once
#include <core/Global_H.hpp>
#include <unordered_map>
NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END
class Shader
{
    public:
        Shader(const char* name, GLenum type);
        ~Shader();
    public:
        auto Getid() const             -> GLuint ;
    private:
        auto Load(const char* name)    -> void ;
        auto LoadSource(const char* full_name_file)         -> void;
        auto Compile()                                      -> void;
        auto checkShaderCompileStatus(const GLuint &shader) -> void;
    
    private:
        GLuint id;
        GLenum Type;
};
