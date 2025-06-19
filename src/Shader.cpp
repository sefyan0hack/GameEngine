#include <core/OpenGL.hpp>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>
#include <core/Shader.hpp>


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
: m_Id(0), m_Type(0), m_Content({})
{
}

Shader::Shader(const char* name, GLenum type)
: m_Id(gl::CreateShader(type)), m_Type(type), m_Content(LoadFile(name))
{
    LoadSource();
    Compile(m_Id);
    checkShaderCompileStatus(m_Id);
    
}

Shader::Shader(const Shader& other)
    : m_Id(gl::CreateShader(other.m_Type))
    , m_Type(other.m_Type)
    , m_Content(other.m_Content)
{
    LoadSource();
    Compile(m_Id);
    checkShaderCompileStatus(m_Id);
    

}

Shader::Shader(Shader&& other) noexcept
    : m_Id(std::exchange(other.m_Id, 0))
    , m_Type(std::exchange(other.m_Type, 0))
    , m_Content(std::exchange(other.m_Content, {}))
{
}

Shader &Shader::operator=(const Shader& other)
{
    if(*this != other){
        this->m_Id = gl::CreateShader(other.m_Type);
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
    gl::DeleteShader(m_Id);
}

auto Shader::LoadSource() -> void
{
    const auto ShaderSource = this->m_Content.data();
    const auto size = static_cast<GLint>(this->m_Content.size());
    gl::ShaderSource(m_Id, 1, &ShaderSource, &size);
}

auto Shader::LoadSource(const std::vector<GLchar>& src, GLuint shader) -> void
{
    const auto ShaderSource = src.data();
    const auto size = static_cast<GLint>(src.size());
    gl::ShaderSource(shader, 1, &ShaderSource, &size);
}

auto Shader::LoadFile(const char* filename) -> std::vector<GLchar>
{
    auto buffer = std::stringstream{};
    auto result = std::vector<GLchar>{};
    GLint m_Major{}, m_Minor{};

    gl::GetIntegerv(GL_MAJOR_VERSION, &m_Major);
    gl::GetIntegerv(GL_MINOR_VERSION, &m_Minor);

    auto glsl_verion = std::format("{}{}0", m_Major, m_Minor);

    buffer << std::format("#version {} {}\n", glsl_verion, sys::Target == sys::Target::Web ? "es" : "core");
    buffer << std::format("precision {} float;\n", sys::Target == sys::Target::Web ? "mediump" : "highp");

    //todo : size increment insted of  buff.str()
    auto op = file_to_str(filename);
    if(op)
    {
        buffer << op.value();

        auto str = buffer.str();
        result.resize(str.size());
        memcpy(result.data(), str.data(), str.size());

        Info("Loding {} : {} bytes ", filename, str.size());
    }else{
        Error("Couldnt open file {} : {}", filename, strerror_());
    }

    return result;
}

auto Shader::Compile(GLuint shader) -> void
{
    gl::CompileShader(shader);
}

auto Shader::checkShaderCompileStatus(const GLuint &shader) -> void
{
    GLint success = 0;
    gl::GetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        std::memset(infoLog, '\0', 512);
        gl::GetShaderInfoLog(shader, 512, NULL, infoLog);
        gl::DeleteShader(shader);
        Error("SHADER::COMPILATION_FAILED {}", infoLog);
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