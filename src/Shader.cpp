#include <core/OpenGL.hpp>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>
#include <core/Buffer.hpp>
#include <core/Shader.hpp>


namespace {

constexpr auto to_string(GLenum type) -> const char*
{
  switch(type){
    case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
    case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
    case GL_GEOMETRY_SHADER: return "GL_GEOMETRY_SHADER";
    default: return "UNKNOWN";
  }
}

}

Shader::Shader()
: m_Id(0), m_Type(0), m_File(""), m_Content({})
{
    Info("{}", *this);
}

Shader::Shader(const char* name, GLenum type)
    : m_Id(gl::CreateShader(type))
    , m_Type(type)
    , m_File(name)
    , m_Content(LoadFile(name))
{
    LoadSource();
    Compile(m_Id);
    checkShaderCompileStatus(*this);
    Info("{}", *this);
}

// Shader::Shader(const Shader& other)
//     : m_Id(gl::CreateShader(other.m_Type))
//     , m_Type(other.m_Type)
//     , m_File(other.m_File)
//     , m_Content(other.m_Content)
// {
//     LoadSource();
//     Compile(m_Id);
//     checkShaderCompileStatus(*this);
// }

// Shader &Shader::operator=(const Shader& other)
// {
//     if(this != &other){
//         this->m_Id = gl::CreateShader(other.m_Type);
//         this->m_Type = other.m_Type;
//         this->m_File = other.m_File;
//         this->m_Content = other.m_Content;
        
//         LoadSource();
//         Compile(m_Id);
//         checkShaderCompileStatus(*this);
//     }
    
//     return *this;
// }

Shader::Shader(Shader&& other) noexcept
    : m_Id(std::exchange(other.m_Id, 0))
    , m_Type(std::exchange(other.m_Type, 0))
    , m_File(std::exchange(other.m_File, ""))
    , m_Content(std::exchange(other.m_Content, {}))
{
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

auto Shader::PreProcess() -> std::string
{
    auto result = std::string();
    GLint m_Major{}, m_Minor{};

    gl::GetIntegerv(GL_MAJOR_VERSION, &m_Major);
    gl::GetIntegerv(GL_MINOR_VERSION, &m_Minor);

    auto glsl_verion = std::format("{}{}0", m_Major, m_Minor);

    result += std::format("#version {} {}\n", glsl_verion, sys::Target == sys::Target::Web ? "es" : "core");
    result += std::format("precision {} float;\n", sys::Target == sys::Target::Web ? "mediump" : "highp");
    return result;
}

auto Shader::LoadFile(const char* filename) -> std::vector<GLchar>
{
    auto fileContent = utils::file_to_str(filename);

    std::string preprocessed = PreProcess();
    
    size_t totalSize = preprocessed.size() + fileContent.size();
    
    std::vector<GLchar> result;
    result.reserve(totalSize + 1);

    result.insert(result.end(), preprocessed.begin(), preprocessed.end());
    result.insert(result.end(), fileContent.begin(), fileContent.end());
    
    result.push_back('\0');

    Info("Loaded {}: ({} bytes)", filename, totalSize);
    return result;
}

auto Shader::Compile(GLuint shader) -> void
{
    gl::CompileShader(shader);
}

auto Shader::checkShaderCompileStatus(const Shader &shader) -> void
{
    auto id = shader.id();
    GLint success = GetShaderInfo(id, GL_COMPILE_STATUS);

    if (!success) {
        GLint infologlength = GetShaderInfo(id, GL_INFO_LOG_LENGTH);

        if(infologlength > 0){
            std::string infoLog(infologlength, '\0');

            gl::GetShaderInfoLog(id, infologlength, nullptr, infoLog.data());
            gl::DeleteShader(id);
            throw CException("[{}] {} \n", shader.File(), infoLog);
        }
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

auto Shader::File() const -> std::string
{
    return m_File;
}

auto Shader::Content() const -> std::vector<GLchar>
{
    return m_Content;
}

auto Shader::GetShaderInfo(GLuint id, GLenum what) -> GLint
{
    constexpr auto INVALID = std::numeric_limits<GLint>::max();

    GLint result = INVALID;

    gl::GetShaderiv(id, what, &result);

    if(result != INVALID)
        return result;
    else
        throw CException("GetShaderiv Failed");
}