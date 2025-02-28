#include <core/gl.h>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>
#include <core/Shader.hpp>
#include <string>
#include <fstream>
#include <sstream>

namespace {

constexpr auto to_string(GLenum type) -> const char*
{
  switch(type){
    case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
    case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
    case GL_COMPUTE_SHADER: return "GL_COMPUTE_SHADER";
    case GL_GEOMETRY_SHADER: return "GL_GEOMETRY_SHADER";
    case GL_TESS_CONTROL_SHADER: return "GL_TESS_CONTROL_SHADER";
    case GL_TESS_EVALUATION_SHADER: return "GL_TESS_EVALUATION_SHADER";
    default: return "UNKNOWN";
  }
}

}

Shader::Shader()
{
}

Shader::Shader(const char* name, GLenum type)
: m_Id(glCreateShader(type)), m_Type(type), m_Content(LoadFile(name))
{
    LoadSource();
    Compile(m_Id);
    checkShaderCompileStatus(m_Id);
    Log::Info("{}", *this);
}

Shader::Shader(const Shader& other)
    : m_Id(glCreateShader(other.m_Type))
    , m_Type(other.m_Type)
    , m_Content(other.m_Content)
{
    LoadSource();
    Compile(m_Id);
    checkShaderCompileStatus(m_Id);
    Log::Info("{}", *this);

}

Shader::Shader(Shader&& other)
    : m_Id(other.m_Id)
    , m_Type(other.m_Type)
    , m_Content(std::move(other.m_Content))
{
    other.m_Id = 0;
    other.m_Type = 0;
    other.m_Content.clear();
}

Shader &Shader::operator=(const Shader& other)
{
    if(*this != other){
        this->m_Id = glCreateShader(other.m_Type);
        this->m_Type = other.m_Type;
        this->m_Content = other.m_Content;
        
        LoadSource();
        Compile(m_Id);
        checkShaderCompileStatus(m_Id);
    }
    
    return *this;
}
bool Shader::operator==(const Shader &other)
{
    return this->m_Id == other.m_Id;
}
Shader::~Shader()
{
    // if(glIsShader(id) == GL_TRUE) glDeleteShader(id);
}

auto Shader::LoadSource() -> void
{
    const auto ShaderSource = this->m_Content.data();
    const auto size = static_cast<GLint>(this->m_Content.size());
    glShaderSource(m_Id, 1, &ShaderSource, &size);
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


auto Shader::id() const -> GLuint
{
    return m_Id;
}

auto Shader::Type() const -> GLenum
{
    return m_Type;
}

auto Shader::TypeName() const -> const char*
{
    return to_string(m_Type);
}

auto Shader::Content() const -> std::vector<GLchar>
{
    return m_Content;
}