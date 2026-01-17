#include <core/Log.hpp>
#include <core/SysInfo.hpp>
#include "OpenGL.hpp"
#include "Shader.hpp"


std::string Shader::glsl_header = std::format(
    "#version {}{}0 {}\n"
    "precision {} float;\n",
    gl::OPENGL_MAJOR_VERSION, gl::OPENGL_MINOR_VERSION, 
    gl::api == gl::API::ES ? "es" : "core",
    gl::api == gl::API::ES ? "mediump" : "highp"
);

constexpr auto glsl_lib = R"(
//Luminance (Grayscale conversion)
float luminance(vec3 color) {
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}
// UV Tiling and Offset
vec2 scaleOffset(vec2 uv, vec2 scale, vec2 offset) {
    return uv * scale + offset;
}
// Saturation
vec3 saturate(vec3 rgb, float adjustment) {
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(rgb, W));
    return mix(intensity, rgb, adjustment);
}
)";

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
    auto srcs = {glsl_header.c_str(), glsl_lib, utils::file_to_str(filename.c_str()).c_str()};
    
    set_sources(srcs);
    compile();
    check_compile_status();

    CTOR_LOG
}

Shader::Shader(std::string Src, GLenum type)
{
    m_Id = gl::CreateShader(type);
    m_Type = type;
    auto srcs = {glsl_header.c_str(), glsl_lib, Src.c_str()};

    set_sources(srcs);
    compile();
    check_compile_status();

    CTOR_LOG
}

Shader::Shader(const cmrc::file& Src, GLenum type)
    : Shader(std::string(Src.begin(), Src.end()), type)
{}

auto Shader::set_sources(const std::span<const char* const> srcs) const -> void
{
    gl::ShaderSource(m_Id, static_cast<GLsizei>(srcs.size()), srcs.data(), nullptr);
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
    switch(m_Type){
        case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
        default: return "UNKNOWN";
    }
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
