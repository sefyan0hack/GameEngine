#include <core/gl.h>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>
#include <core/Shader.hpp>
#include <string>
#include <fstream>
#include <sstream>

Shader::Shader(const char* name, GLenum type)
: id(glCreateShader(type)), Type(type)
{
    Load(name);
    Log::Info("{}", *this);
}

Shader::~Shader()
{
    glDeleteShader(id);
}

auto Shader::LoadSource(const char* name) -> void
{

    auto shader_file = std::ifstream(name);
    auto buffer = std::ostringstream{};

    if( not shader_file.is_open()){
        Log::Error("Open {} Failed. code: {}", name, errno);
    }
    Log::Info("Loding {}", name);
    buffer << shader_file.rdbuf();
    std::string strFile = buffer.str();

    const char* ShaderSource = strFile.c_str();
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