#include <core/Log.hpp>
#include <core/SysInfo.hpp>
#include "OpenGL.hpp"
#include "Shader.hpp"

extern cmrc::embedded_filesystem embed_filesystem;

namespace {

constexpr auto to_string(GLenum type) -> const char*
{
  switch(type){
    case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
    case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
    default: return "UNKNOWN";
  }
}

}

Shader::Shader()
    : m_Id(0), m_Type(0)
{
    CTOR_LOG
}

Shader::Shader(Shader&& other) noexcept : m_Id(other.m_Id), m_Type(other.m_Type) {
    other.m_Id = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        gl::DeleteShader(m_Id);
        m_Id = other.m_Id;
        m_Type = other.m_Type;
        other.m_Id = 0;
    }
    return *this;
}

Shader::~Shader(){
    gl::DeleteShader(m_Id);

    DTOR_LOG
}


Shader::Shader(const std::string& filename)
{
    if(filename.ends_with(".vert")){
        m_Type = GL_VERTEX_SHADER;
    } else if(filename.ends_with(".frag")){
        m_Type = GL_FRAGMENT_SHADER;
    } else {
        throw Exception("filename `{}` must be one of [.vert, .frag]", filename);
    }

    m_Id = gl::CreateShader(m_Type);
    auto src = pre_process() + utils::file_to_str(filename.c_str());

    set_source(src);
    compile();
    check_compile_status();

    CTOR_LOG
}

Shader::Shader(std::string Src, GLenum type)
{
    m_Id = gl::CreateShader(type);
    m_Type = type;
    auto src = pre_process() + Src;

    set_source(src);
    compile();
    check_compile_status();

    CTOR_LOG
}

Shader::Shader(const cmrc::file& Src, GLenum type)
    : Shader(std::string(Src.begin(), Src.end()), type)
{}



auto Shader::set_source(const std::string& src) const -> void
{
    const auto ShaderSource = src.data();
    const auto size = static_cast<GLint>(src.size());
    gl::ShaderSource(m_Id, 1, &ShaderSource, &size);
}

auto Shader::pre_process() -> std::string
{
    auto result = std::string();

    GLint m_Major = gl::get_integer(GL_MAJOR_VERSION);

    GLint m_Minor = gl::get_integer(GL_MINOR_VERSION);
    auto glsl_verion = std::format("{}{}0", m_Major, m_Minor);

    result += std::format("#version {} {}\n", glsl_verion, os::host::name_tag() == os::Target::Web ? "es" : "core");
    result += std::format("precision {} float;\n", os::host::name_tag() == os::Target::Web ? "mediump" : "highp");
    return result;
}

auto Shader::compile() -> void
{
    gl::CompileShader(m_Id);
}

auto Shader::check_compile_status() -> void
{
    auto success = get_shader_info(GL_COMPILE_STATUS);

    if (!success) {
        auto infologlength = get_shader_info(GL_INFO_LOG_LENGTH);

        if(infologlength > 0){
            std::string infoLog(infologlength, '\0');

            gl::GetShaderInfoLog(m_Id, infologlength, nullptr, infoLog.data());
            gl::DeleteShader(m_Id);
            throw Exception("{}", infoLog);
        }
    }
}

auto Shader::id() const -> GLuint
{
    return m_Id;
}

auto Shader::type() const -> GLenum
{
    return m_Type;
}

auto Shader::type_name() const -> const char*
{
    return to_string(m_Type);
}

auto Shader::get_shader_info(GLenum what) const -> GLint
{
    constexpr auto INVALID = std::numeric_limits<GLint>::max();

    GLint result = INVALID;

    gl::GetShaderiv(m_Id, what, &result);

    if(result != INVALID)
        return result;
    else
        throw Exception("GetShaderiv Failed");
}

auto Shader::default_vert() -> std::shared_ptr<Shader>
{
    static auto v = std::make_shared<Shader>(embed_filesystem.open("res/Shaders/main.vert"), GL_VERTEX_SHADER);
    return v;
}

auto Shader::default_frag() -> std::shared_ptr<Shader>
{
    static auto f = std::make_shared<Shader>(embed_filesystem.open("res/Shaders/main.frag"), GL_FRAGMENT_SHADER);
    return f;
}

