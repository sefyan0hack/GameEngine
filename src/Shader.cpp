#include <core/gl.h>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>
#include <core/Shader.hpp>
#include <string>
#include <fstream>

Shader::Shader()
: id(0), Type(0)
{
}

Shader::Shader(const char* name, GLenum type)
: id(glCreateShader(type)), Type(type)
{
    LoadFile(name);
    LoadSource();
    Compile();
    checkShaderCompileStatus(id);
    Log::Info("{}", *this);
}

Shader::Shader(const Shader& other)
    : id(glCreateShader(other.Type))
    , Type(other.Type)
    , Content(other.Content)
{
    Compile();
    //no need for check status
    Log::Info("{}", *this);

}

Shader::Shader(Shader&& other)
    : id(other.id)
    , Type(other.Type)
    , Content(std::move(other.Content))
{
}

Shader &Shader::operator=(const Shader& other)
{
    if(*this != other){
        this->id = glCreateShader(other.Type);
        this->Type = other.Type;
        this->Content = other.Content;
        Compile();
        //no need for check status
    }
    
    return *this;
}
bool Shader::operator==(const Shader &other)
{
    return this->id == other.id;
}
Shader::~Shader()
{
    if(glIsShader(id) == GL_TRUE) glDeleteShader(id);
}

auto Shader::LoadSource() -> void
{
    const auto ShaderSource = this->Content.data();
    const auto size = static_cast<GLint>(this->Content.size());
    glShaderSource(id, 1, &ShaderSource, &size);
}

auto Shader::LoadFile(const char* filename) -> void
{
    auto shader_file = std::ifstream(filename, std::ios::binary | std::ios::ate);

    if( not shader_file.is_open()){
        Log::Error("Open {} Failed. code: {}", filename, errno);
    }

    auto size = shader_file.tellg();
    shader_file.seekg(0, std::ios::beg);
    if (size < 0) {
        Log::Error("Failed to get size for {}", filename);
    }

    this->Content.resize(static_cast<std::size_t>(size));

    if ( not shader_file.read(this->Content.data(), size)) {
        Log::Error("Reading {} failed.", filename);
    }

    shader_file.close();
    Log::Info("Loding {} ", filename);
}

auto Shader::Compile() -> void
{
    Log::Info("Compiling Shader id: {}, Type: {}", id, GetTypeName());
    LoadSource();
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