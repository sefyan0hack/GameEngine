#include <core/gl.h>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>
#include <core/Shader.hpp>
#include <string>
#include <fstream>
#include <sstream>

Shader::Shader()
{
}

Shader::Shader(const char* name, GLenum type)
: id(glCreateShader(type)), Type(type), Content(LoadFile(name))
{
    LoadSource();
    Compile(id);
    checkShaderCompileStatus(id);
    Log::Info("{}", *this);
}

Shader::Shader(const Shader& other)
    : id(glCreateShader(other.Type))
    , Type(other.Type)
    , Content(other.Content)
{
    LoadSource();
    Compile(id);
    checkShaderCompileStatus(id);
    Log::Info("{}", *this);

}

Shader::Shader(Shader&& other)
    : id(other.id)
    , Type(other.Type)
    , Content(std::move(other.Content))
{
    other.id = 0;
    other.Type = 0;
    other.Content.clear();
}

Shader &Shader::operator=(const Shader& other)
{
    if(*this != other){
        this->id = glCreateShader(other.Type);
        this->Type = other.Type;
        this->Content = other.Content;
        
        LoadSource();
        Compile(id);
        checkShaderCompileStatus(id);
    }
    
    return *this;
}
bool Shader::operator==(const Shader &other)
{
    return this->id == other.id;
}
Shader::~Shader()
{
    // if(glIsShader(id) == GL_TRUE) glDeleteShader(id);
}

auto Shader::LoadSource() -> void
{
    const auto ShaderSource = this->Content.data();
    const auto size = static_cast<GLint>(this->Content.size());
    glShaderSource(id, 1, &ShaderSource, &size);
}

auto Shader::LoadSource(const std::vector<GLchar>& src, GLuint shader) -> void
{
    const auto ShaderSource = src.data();
    const auto size = static_cast<GLint>(src.size());
    glShaderSource(shader, 1, &ShaderSource, &size);
}

auto Shader::LoadFile(const char* filename) -> std::vector<GLchar>
{
    auto file = std::ifstream{};
    auto buffer = std::stringstream{};
    auto result = std::vector<GLchar>{};

    file.open(filename);
    if(file.is_open())
    {
        buffer << file.rdbuf();
        auto str = buffer.str();
        str += '\0';
        result.resize(str.size());
        memcpy(result.data(), str.data(), str.size());
        Log::Info("Loding {} ", filename);
    }else{
        Log::Error("Couldnt open file {} : {}", filename, errno);
    }

    file.close();
    return result;
}

auto Shader::Compile(GLuint shader) -> void
{
    glCompileShader(shader);
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

auto Shader::GetContent() const -> std::vector<GLchar>
{
    return Content;
}