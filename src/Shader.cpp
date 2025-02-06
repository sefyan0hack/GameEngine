#include <core/gl.h>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>
#include <core/Shader.hpp>
#include <string>
#include <fstream>

Shader::Shader()
: id(0), Type(0), Content("")
{
}

Shader::Shader(const char* name, GLenum type)
: id(glCreateShader(type)), Type(type), Content("")
{
    LoadFile(name);
    LoadSource();
    Compile();
    checkShaderCompileStatus(id);
    Log::Info("{}", *this);
}

Shader::Shader(const Shader& other)
    : id(glCreateShader(other.GetType()))
    , Type(other.GetType())
    , Content(other.GetContent())
{
    Compile();
    //no need for check status
    Log::Info("{}", *this);

}

Shader::Shader(Shader&& other)
    : id(other.Getid())
    , Type(other.GetType())
    , Content(std::move(other.GetContent()))
{
}

Shader &Shader::operator=(const Shader& other)
{
    if(*this != other){
        this->id = glCreateShader(other.GetType());
        this->Type = other.GetType();
        this->Content = other.GetContent();
        Compile();
        //no need for check status
    }else{
        return *this;
    }
}
bool Shader::operator==(const Shader &other)
{
    return this->id == other.id;
}
Shader::~Shader()
{
    if(id != 0)
        glDeleteShader(id);
}

auto Shader::LoadSource() -> void
{
    const char* ShaderSource = this->Content.c_str();
    glShaderSource(id, 1, &ShaderSource, NULL);
}

auto Shader::LoadFile(const char* filenmae) -> void
{
    auto shader_file = std::ifstream(filenmae, std::ios::binary | std::ios::ate);
    auto size = shader_file.tellg();

    if( not shader_file.is_open()){
        Log::Error("Open {} Failed. code: {}", filenmae, errno);
    }

    this->Content.resize(size);

    shader_file.seekg(0);
    shader_file.read(this->Content.data(), size);

    Log::Info("Loding {} ", filenmae);
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
auto Shader::GetContent() const -> std::string
{
    return Content;
}