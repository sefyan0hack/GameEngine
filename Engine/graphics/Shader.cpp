#include <core/Log.hpp>
#include <core/SysInfo.hpp>
#include <core/Exception.hpp>
#include <core/res.hpp>
#include <core/Utils.hpp>

#include "OpenGL.hpp"
#include "Shader.hpp"

#include <ranges>

Shader::Shader(const char* shader, GLenum type)
{
    m_Id = gl::CreateShader(type);
    m_Type = type;

    auto glsl_h = glsl_header();
    auto glsl_l = glsl_lib();
    auto src = res::get(shader);

    auto srcs = {glsl_h.c_str(), glsl_l.c_str(), src.data()};
    
    set_sources(srcs);
    compile();

    auto comp_stat = check_compile_status();
    if (!comp_stat.empty()) {
        std::string line = "0";
        std::string msg;
        auto lines = comp_stat | std::views::split('\n') | std::ranges::to<std::vector<std::string>>();
        for(auto l : lines){
            if(l.starts_with("ERROR:")){ // probably intel
                auto p1 = l.find(':');          // ERROR:
                auto p2 = l.find(':', p1 + 1);  // source id
                auto p3 = l.find(':', p2 + 1);  // line number
                
                line = l.substr(p2 + 1, p3 - p2 - 1);
                msg  = l.substr(p3 + 1);
            }
            if(!msg.empty())
                logg::error("\n\t-> glsl compile : {}:{} {}", shader, line, msg);

            line = "0";
            msg.clear();
        }
    }
}

Shader::Shader(const std::string& shader, GLenum type) : Shader(shader.c_str(), type) {}

Shader::~Shader(){
    gl::DeleteShader(m_Id);
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

Shader::Shader(Shader&& other) noexcept : m_Id(other.m_Id), m_Type(other.m_Type) {
    other.m_Id = 0;
}

auto Shader::set_sources(const std::span<const char* const> srcs) const -> void
{
    gl::ShaderSource(m_Id, static_cast<GLsizei>(srcs.size()), srcs.data(), nullptr);
}

auto Shader::compile() -> void
{
    gl::CompileShader(m_Id);
}

auto Shader::check_compile_status() -> std::string
{
    auto success = get_shader_info(GL_COMPILE_STATUS);
    GLchar* buffer = nullptr;

    if (!success) {
        auto infologlength = static_cast<GLsizei>(get_shader_info(GL_INFO_LOG_LENGTH));

        if(infologlength > 0){
            buffer = new GLchar[infologlength];

            gl::GetShaderInfoLog(m_Id, infologlength, nullptr, buffer);
            gl::DeleteShader(m_Id);
        }
    }

    auto r = buffer ? std::string{buffer}: std::string{};
    if(!r.empty()) delete[] buffer;
    return r;
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
    GLint result = -1;

    gl::GetShaderiv(m_Id, what, &result);

    if(result != -1)
        return result;
    else
        throw Exception("GetShaderiv Failed");
}

auto Shader::new_vertex(const std::string& vert) -> std::shared_ptr<Shader>
{
  return std::make_shared<Shader>(vert, GL_VERTEX_SHADER);
}

auto Shader::new_fragment(const std::string& frag) -> std::shared_ptr<Shader>
{
  return std::make_shared<Shader>(frag, GL_FRAGMENT_SHADER);
}

auto Shader::glsl_header() -> std::string
{
    return std::format(
        "#version {}{}0 {}\n"
        "precision {} float;\n",
        gl::MIN_REQUIRED_MAJOR_VERSION, gl::MIN_REQUIRED_MINOR_VERSION, 
        gl::api == gl::API::ES ? "es" : "core",
        gl::api == gl::API::ES ? "mediump" : "highp"
    );
}

auto Shader::glsl_lib() -> std::string
{
    return
R"(
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
#line 1 // The #line statement is there so that compiler error messages give the correct line numbers
)";
}