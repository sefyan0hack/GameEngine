#include <glad/glad.h>
#include <string>
#include <core/Global_H.hpp>
#include <core/Shader.hpp>
#include <glm/glm.hpp>

Shader::Shader(const char* name, GLenum type)
: id(glCreateShader(type)), Type(type)
{
    Load(name);
}

Shader::~Shader()
{
    glDeleteShader(id);
}

auto Shader::LoadSource(const char* full_name_file) -> void
{

    FILE *shader_file;

    shader_file = fopen(full_name_file, "r+b");
    
    if(shader_file == nullptr){
        ERR("Open " << full_name_file <<" Failed. code: " << errno);
    }

    LOG("[+] Loding " << full_name_file );

    std::string shader_data ;
    char  byte;
    int i = 0;
    while (fread(&byte, sizeof(char), 1, shader_file))
    {
        shader_data.resize(shader_data.size() + 1);
        shader_data.at(i++) = byte;
    }


    if(fclose(shader_file) != 0 ){
        ERR("Closing Files erro");
    }

    const char* ShaderSource = shader_data.c_str();
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