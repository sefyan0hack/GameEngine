#include <core/gl.h>
#include <string>
#include <core/Log.hpp>
#include <core/Shader.hpp>
#include <glm/glm.hpp>
#include <fstream>
#include <format>

Shader::Shader(const char* name, GLenum type)
: id(glCreateShader(type)), Type(type)
{
    Load(name);
}

Shader::~Shader()
{
    glDeleteShader(id);
}

auto Shader::LoadSource(const char* name) -> void
{

    std::ifstream shader_file(name);
    if( not shader_file.is_open()){
        Log::Error("Open {} Failed. code: {}", std::string{name}, int{errno});
    }

    std::string buffer = std::format("#version {}{}0 core\n", GLVersion.major, GLVersion.minor);
    buffer.append(std::istreambuf_iterator<char>(shader_file), std::istreambuf_iterator<char>());

    Log::Info("[+] Loding {}", std::string{name} );

    const char* ShaderSource = buffer.c_str();
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