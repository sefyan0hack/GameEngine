#include <glad/glad.h>
#include <string>
#include <core/Global_H.hpp>
#include <core/Shader.hpp>
#include <glm/glm.hpp>
#include <fstream>

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
    std::string buffer = "#version " + std::to_string(GLVersion.major) + std::to_string(GLVersion.minor) + "0 core\n";
    std::string myText;
    while (std::getline(shader_file, myText)) {
        buffer += myText + "\n";
    }

    if( not shader_file.is_open()){
        ERR("Open " << name <<" Failed. code: " << errno);
    }

    LOG("[+] Loding " << name );
    LOG(buffer);

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
        ERR("SHADER::COMPILATION_FAILED\n" << infoLog );
        glDeleteShader(shader); // Don't leak the shader.
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