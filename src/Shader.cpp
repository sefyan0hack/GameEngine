#include <core/gl.h>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>
#include <core/Shader.hpp>
#include <string>
#include <fstream>

Shader::Shader(const char* name, GLenum type)
: id(glCreateShader(type)), Type(type), Content("")
{
    Load(name);
    Log::Info("{}", *this);
}

Shader::~Shader()
{
    if(id != 0)
        glDeleteShader(id);
}

auto Shader::LoadSource(const char* name) -> void
{

    auto shader_file = std::ifstream(name, std::ios::binary | std::ios::ate);
    auto size = shader_file.tellg();

    if( not shader_file.is_open()){
        Log::Error("Open {} Failed. code: {}", name, errno);
    }

    this->Content.resize(size);

    shader_file.seekg(0);
    shader_file.read(this->Content.data(), size);

    Log::Info("Loding {} ", name);
    const char* ShaderSource = this->Content.c_str();
    glShaderSource(id, 1, &ShaderSource, NULL);
}

auto Shader::Compile() -> void
{
    glCompileShader(id);
}

auto Shader::checkShaderCompileStatus(const GLuint &shader) -> void
{
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        // ERR("SHADER::COMPILATION_FAILED\n" << infoLog );
        glDeleteShader(shader); // Don't leak the shader.
        Log::Error("SHADER::COMPILATION_FAILED {}", infoLog);
    }
}



auto Shader::Load(const char* name) -> void
{
    LoadSource(name);
    Compile();
    Shader::checkShaderCompileStatus(id);
}

auto Shader::Getid() const -> GLuint
{
    return id;
}

auto Shader::GetType() const -> GLenum
{
    return Type;
}

auto Shader::GetTypeName() const -> const char*
{
    return SHADERTYPES[Type];
}