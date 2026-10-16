#include <core/Log.hpp>
#include <core/SysInfo.hpp>
#include <core/Exception.hpp>
#include <core/res.hpp>
#include <core/Utils.hpp>

#include "gl.hpp"
#include "OpenGL.hpp"
#include "Shader.hpp"

#include <ranges>
#include <sstream>

auto get_shader_info(uint32_t id, uint32_t what) -> int32_t; //what : GL_SHADER_TYPE, GL_DELETE_STATUS, GL_COMPILE_STATUS, GL_INFO_LOG_LENGTH, GL_SHADER_SOURCE_LENGTH.


Shader::Shader(const char* shader, Type type)
{
    uint32_t gl_type{};

    switch (type){
        case Type::Vertex: gl_type = GL_VERTEX_SHADER; break;
        case Type::Fragment: gl_type = GL_FRAGMENT_SHADER; break;
        default: gl_type = 0;
    }

    m_Id = gl::create_shader(gl_type, shader);
    m_Type = type;

    std::ostringstream header;

    header << "#version " << OpenGL::MIN_REQUIRED_MAJOR_VERSION << OpenGL::MIN_REQUIRED_MINOR_VERSION << "0 " << ( OpenGL::api == OpenGL::API::ES ? "es\n" : "core\n");
    header << "precision mediump float;\n";
    header << "precision mediump int;\n";
    header << "precision mediump sampler2D;\n";
    header << "precision mediump sampler3D;\n";
    header << "precision mediump samplerCube;\n";

    header << "#pragma optimize("<< (OpenGL::DEBUG ? "off" : "on") <<")\n";
    header << "#pragma debug("<< (OpenGL::DEBUG ? "off" : "on") <<")\n";

    // Macros & defines
    header << "#define MAX_FRAGMENT_TEXTURE_UNITS " << OpenGL::MAX_FRAGMENT_TEXTURE_UNITS << '\n';
    header << "#define MAX_VERTEX_TEXTURE_UNITS " << OpenGL::MAX_VERTEX_TEXTURE_UNITS << '\n';
    header << "#define MAX_COMBINED_TEXTURE_UNITS " << OpenGL::MAX_COMBINED_TEXTURE_UNITS << '\n';

    header << res::get("res/shaders/defines.h").data() << '\n';

    {
        header << "#define sampler_at(s, uv, idx) (";
        for (int i = 0; i < OpenGL::MAX_FRAGMENT_TEXTURE_UNITS; i++)
            header << "idx == " << i << " ? texture(s[" << i << "], uv) : ";

        header << "vec4(0.0))";
    }

    auto glsl_header = header.str();
    auto glsl_l = res::get("res/shaders/common.glsl");
    auto glsl_src = res::get(shader);

    auto srcs = {glsl_header.c_str(), glsl_l.data(), glsl_src.data()};
    
    set_sources(srcs);
    compile();

    static auto vndr = reinterpret_cast<const char*>(gl::GetString(GL_VENDOR));
    static auto vendor = std::string(vndr ? vndr : "");

    auto comp_stat = check_compile_status();

    if (!comp_stat.empty()) {
        for(auto r : comp_stat | std::views::split('\n')){
            std::string line = "0";
            std::string msg;

            std::string l(r.begin(), r.end());
            if(vendor.contains("Intel")){ // ERROR:soure:line:
                auto p1 = l.find(':');          // ERROR:
                auto p2 = l.find(':', p1 + 1);  // source id
                auto p3 = l.find(':', p2 + 1);  // line number

                line = l.substr(p2 + 1, p3 - p2 - 1);
                msg  = l.substr(p3 + 1);
            } else if(vendor.contains("ARM")) { // :soure:line: L000x:
                auto p2 = l.find(':',  1);  // source id
                auto p3 = l.find(':', p2 + 1);  // line number

                line = l.substr(p2 + 1, p3 - p2 - 1);
                msg  = l.substr(p3 + 1);
            } else {
                line = "0";
                msg = l;
            }

            if(!msg.empty())
                logg::error("\n\t-> glsl compiler : {}:{} {}", shader, line, msg);
        }
    }
}

Shader::Shader(const std::string& shader, Type type)
    : Shader(shader.c_str(), type)
{}

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
    gl::ShaderSource(m_Id, static_cast<int32_t>(srcs.size()), srcs.data(), nullptr);
}

auto Shader::compile() -> void
{
    gl::CompileShader(m_Id);
}

auto Shader::check_compile_status() -> std::string
{
    auto success = get_shader_info(m_Id, GL_COMPILE_STATUS);
    char* buffer = nullptr;

    if (!success) {
        auto infologlength = static_cast<int32_t>(get_shader_info(m_Id, GL_INFO_LOG_LENGTH));

        if(infologlength > 0){
            buffer = new char[infologlength];

            gl::GetShaderInfoLog(m_Id, infologlength, nullptr, buffer);
            // gl::DeleteShader(m_Id);
        }
    }

    auto r = buffer ? std::string{buffer}: std::string{};
    if(!r.empty()) delete[] buffer;
    return r;
}

auto Shader::id() const -> uint32_t
{
    return m_Id;
}

auto Shader::type() const -> Type
{
    return m_Type;
}

auto Shader::type_name() const -> const char*
{
    switch(m_Type){
        case Type::Vertex: return "Vertex";
        case Type::Fragment: return "Fragment";
        default: return "UNKNOWN";
    }
}


auto Shader::new_vertex(const std::string& vert) -> std::shared_ptr<Shader>
{
    return std::make_shared<Shader>(vert, Type::Vertex);
}

auto Shader::new_fragment(const std::string& frag) -> std::shared_ptr<Shader>
{
  return std::make_shared<Shader>(frag, Type::Fragment);
}

auto get_shader_info(uint32_t id, uint32_t what) -> int32_t
{
    int32_t result = -1;

    gl::GetShaderiv(id, what, &result);

    if(result != -1)
        return result;
    else
        throw Exception("GetShaderiv Failed");
}