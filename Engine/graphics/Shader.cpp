#include <core/Log.hpp>
#include <core/SysInfo.hpp>
#include <core/Exception.hpp>
#include <core/res.hpp>
#include <core/Utils.hpp>

#include "Shader.hpp"
#include "OpenGL.hpp"
#include "gl.hpp"

#include <ranges>

Shader::Shader(const char* shader, GLenum type)
{
    m_Id = gl::CreateShader(type);
    m_Type = type;

    auto glsl_header = std::format(
        "#version {}{}0 {}\n"
        "precision mediump float;\n",
        OpenGL::MIN_REQUIRED_MAJOR_VERSION, OpenGL::MIN_REQUIRED_MINOR_VERSION, 
        OpenGL::api == OpenGL::API::ES ? "es" : "core"
    );

    auto glsl_l = res::get("res/Shaders/common.glsl");
    auto glsl_src = res::get(shader);

    auto srcs = {glsl_header.c_str(), glsl_l.data(), glsl_src.data()};
    
    set_sources(srcs);
    compile();

    auto comp_stat = check_compile_status();
    if (!comp_stat.empty()) {
        std::string line = "0";
        std::string msg;

        for(auto r : comp_stat | std::views::split('\n')){
            std::string l(r.begin(), r.end());
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

    gl::label_shader(m_Id, shader);
}

Shader::Shader(const std::string& shader, GLenum type) : Shader(shader.c_str(), type) { gl::label_shader(m_Id, shader.c_str()); }

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
