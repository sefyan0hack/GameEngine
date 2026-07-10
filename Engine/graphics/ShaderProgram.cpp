#include "ShaderProgram.hpp"

#include "gl.hpp"
#include "Shader.hpp"

#include <core/Log.hpp>
#include <core/res.hpp>
#include <core/Exception.hpp>

#include <utility>

ShaderProgram::ShaderProgram(std::shared_ptr<Shader> vertex, std::shared_ptr<Shader> fragment)
    : m_Id(gl::CreateProgram())
{
    m_Shaders.reserve(2);
    m_Shaders.push_back(std::move(vertex));
    m_Shaders.push_back(std::move(fragment));
    
    for(const auto &shader : m_Shaders ){
        gl::AttachShader(m_Id, shader->id());
    }

    link();

    auto link_stat = check_link_status();

    if (!link_stat.empty())
        logg::error("\n\t-> glsl link : {}", link_stat);


    for(const auto &shader : m_Shaders ){
        gl::DetachShader(m_Id, shader->id());
    }

    dump_attribs();
    dump_uniforms();
}

ShaderProgram::ShaderProgram(const char* vertex, const char* fragment)
: ShaderProgram(Shader::new_vertex(vertex), Shader::new_fragment(fragment)) 
{}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : m_Id(std::exchange(other.m_Id, 0))
    , m_Shaders(std::move(other.m_Shaders))
    , m_Attribs(std::move(other.m_Attribs))
    , m_Uniforms(std::move(other.m_Uniforms)) // dnt forget  to check if the id are the same in the new Programe
{
}

auto ShaderProgram::operator=(ShaderProgram&& other) noexcept -> ShaderProgram& 
{
    if (this != &other) {
        gl::DeleteProgram(m_Id);

        m_Id = std::exchange(other.m_Id, 0);
        m_Shaders = std::move(other.m_Shaders);
        m_Attribs = std::move(other.m_Attribs);
        m_Uniforms = std::move(other.m_Uniforms);
    }
    return *this;
}

ShaderProgram::~ShaderProgram()
{
    gl::DeleteProgram(m_Id);
}

auto ShaderProgram::id() const noexcept -> uint32_t
{
    return m_Id;
}

auto ShaderProgram::use() const -> void
{
    gl::UseProgram(m_Id);
}

auto ShaderProgram::link() const -> void
{
    gl::LinkProgram(m_Id);
}

auto ShaderProgram::uniform_location(const char *name) const -> uint32_t
{
    try {
        auto it = m_Uniforms.at(name);
        auto [loc, type, size] = it;
        return loc;
    } catch( const std::exception& e){
        throw Exception("[what : {} ] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::attrib_location(const char *name) const -> uint32_t
{
    try {
        auto [loc, type, size] = m_Attribs.at(name);
        return loc;
    } catch( const std::exception& e){
        throw Exception("[what : {} ] the Attrib `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::uniform_location_prv(const char *name) const -> uint32_t
{
    int32_t location = gl::GetUniformLocation(m_Id, name);
    if (location == -1) {
        throw Exception("uniform {} doesn't exist!", name);
    }
    return static_cast<uint32_t>(location);
}

auto ShaderProgram::attrib_location_prv(const char *name) const -> uint32_t
{
    int32_t location = gl::GetAttribLocation(m_Id, name);
    if (location == -1) {
        throw Exception("Attrib {} doesn't exist!", name);
    }
    return static_cast<uint32_t>(location);
}


auto ShaderProgram::uniform_count() const -> int32_t
{
    int32_t count = get_program_info(GL_ACTIVE_UNIFORMS);

    Expect(count >= 0, "get_program_info(GL_ACTIVE_UNIFORMS) failed retuned : {}", count);

    return count;
}

auto ShaderProgram::attribs_count() const -> int32_t
{
    int32_t count = get_program_info(GL_ACTIVE_ATTRIBUTES);

    Expect(count >= 0, "get_program_info(GL_ACTIVE_ATTRIBUTES) failed retuned : {}", count);

    return count;
}


auto ShaderProgram::dump_uniforms() -> void
{
    int32_t max_len = get_program_info(GL_ACTIVE_UNIFORM_MAX_LENGTH);
        
    if(max_len == 0) {
        throw Exception("error id {}, get_program_info return {}", m_Id, max_len);
    }

    auto count = uniform_count();
    if(count > 0){

        int32_t len = 0;
        int32_t size;
        uint32_t type;

        for(int32_t i = 0; i < count; i++){
            std::string Uniform_name(static_cast<std::size_t>(max_len), '\0');
            gl::GetActiveUniform(m_Id, static_cast<uint32_t>(i), static_cast<int32_t>(max_len), &len, &size, &type, Uniform_name.data());
            if(len > 0) Uniform_name.resize(static_cast<std::size_t>(len));

            // check if uniform belongs to a UBO
            uint32_t index = i;
            int32_t blockIndex = -1;

            gl::GetActiveUniformsiv(
                m_Id,
                1,
                &index,
                GL_UNIFORM_BLOCK_INDEX,
                &blockIndex
            );

            // skip UBO members
            if (blockIndex != -1)
                continue;

            m_Uniforms[Uniform_name] = std::make_tuple(
                static_cast<uint32_t>(uniform_location_prv(Uniform_name.c_str())),
                static_cast<uint32_t>(type),
                static_cast<int32_t>(size)
            );
        }
    }
}

auto ShaderProgram::dump_attribs() -> void
{
    int32_t max_len = get_program_info(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH);
    if(max_len == 0) {
        throw Exception("max_len is not valid max_len: {}", max_len);
    }
    
    auto count = attribs_count();
    if(count > 0){
        int32_t len = 0;
        [[maybe_unused]] int32_t size;
        [[maybe_unused]] uint32_t type;

        for(int32_t i = 0; i < count; i++){
            std::string attrib_name(static_cast<std::size_t>(max_len), '\0');
            gl::GetActiveAttrib(m_Id, static_cast<uint32_t>(i), max_len, &len, &size, &type, attrib_name.data());
            if(len > 0) attrib_name.resize(static_cast<std::size_t>(len));

            if(attrib_name.starts_with("gl_")) continue;

            m_Attribs[attrib_name] = std::make_tuple(
                static_cast<uint32_t>(attrib_location_prv(attrib_name.c_str())),
                static_cast<uint32_t>(type),
                static_cast<int32_t>(size)
            );
        }
    }
}

auto ShaderProgram::current_program() -> uint32_t
{
    int32_t prog = 0;
    gl::GetIntegerv(GL_CURRENT_PROGRAM, &prog);

    return static_cast<uint32_t>(prog);
}

auto ShaderProgram::uniforms() const noexcept -> const std::unordered_map<std::string, InternalType>&
{
    return m_Uniforms;
}

auto ShaderProgram::attribs() const noexcept -> const std::unordered_map<std::string, InternalType>&
{
    return m_Attribs;
}

auto ShaderProgram::get_program_info(uint32_t what) const -> int32_t
{
    int32_t result = -1;

    gl::GetProgramiv(m_Id, what, &result);

    if(result != -1)
        return result;
    else
        throw Exception("gl error id {}", glGetError());
}

auto ShaderProgram::check_link_status() -> std::string
{
    int32_t success = get_program_info(GL_LINK_STATUS);
    char* buffer = nullptr;

    if (!success) {
        auto infologlength = static_cast<int32_t>(get_program_info(GL_INFO_LOG_LENGTH));
    
        if(infologlength > 0){
            buffer = new char[infologlength];
            gl::GetProgramInfoLog(m_Id, infologlength, nullptr, buffer);
        }
    }

    auto r = buffer ? std::string{buffer}: std::string{};
    if(!r.empty()) delete[] buffer;
    return r;
}

///////
auto ShaderProgram::set_uniform(const std::string& name, const int32_t &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform1i(static_cast<int32_t>(loc), value);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const float &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform1f(static_cast<int32_t>(loc), value);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}
auto ShaderProgram::set_uniform(const std::string& name, const uint32_t &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform1ui(static_cast<int32_t>(loc), value);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::vec2 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform2fv(static_cast<int32_t>(loc), 1, &value[0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::vec3 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform3fv(static_cast<int32_t>(loc), 1, &value[0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::vec4 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform4fv(static_cast<int32_t>(loc), 1, &value[0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::ivec2 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform2iv(static_cast<int32_t>(loc), 1, &value[0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::ivec3 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform3iv(static_cast<int32_t>(loc), 1, &value[0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::ivec4 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform4iv(static_cast<int32_t>(loc), 1, &value[0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::uvec2 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform2uiv(static_cast<int32_t>(loc), 1, &value[0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::uvec3 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform3uiv(static_cast<int32_t>(loc), 1, &value[0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::uvec4 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform4uiv(static_cast<int32_t>(loc), 1, &value[0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::mat2 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::UniformMatrix2fv(static_cast<int32_t>(loc), 1, GL_FALSE, &value[0][0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::mat3 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::UniformMatrix3fv(static_cast<int32_t>(loc), 1, GL_FALSE, &value[0][0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::mat4 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::UniformMatrix4fv(static_cast<int32_t>(loc), 1, GL_FALSE, &value[0][0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::set_uniform(const std::string& name, const emath::mat4* value, int32_t count) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size >= count, "GLSL Uniform size:{} < count", size);

        gl::UniformMatrix4fv(static_cast<int32_t>(loc), count, GL_FALSE, &value[0][0][0]);
    } catch(const std::exception& e) {
        throw Exception("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::glsl_type_to_string(uint32_t type) -> const char*
{
    switch (type)
    {
        case GL_FLOAT : return "float";
        case GL_FLOAT_VEC2 : return "vec2";
        case GL_FLOAT_VEC3 : return "vec3";
        case GL_FLOAT_VEC4 : return "vec4";
        case GL_FLOAT_MAT2 : return "mat2";
        case GL_FLOAT_MAT3 : return "mat3";
        case GL_FLOAT_MAT4 : return "mat4";
        case GL_FLOAT_MAT2x3 : return "mat2x3";
        case GL_FLOAT_MAT2x4 : return "mat2x4";
        case GL_FLOAT_MAT3x2 : return "mat3x2";
        case GL_FLOAT_MAT3x4 : return "mat3x4";
        case GL_FLOAT_MAT4x2 : return "mat4x2";
        case GL_FLOAT_MAT4x3 : return "mat4x3";
        case GL_INT : return "int";
        case GL_INT_VEC2 : return "ivec2";
        case GL_INT_VEC3 : return "ivec3";
        case GL_INT_VEC4 : return "ivec4";
        case GL_UNSIGNED_INT : return "unsigned int";
        case GL_UNSIGNED_INT_VEC2 : return "uvec2";
        case GL_UNSIGNED_INT_VEC3 : return "uvec3";
        case GL_UNSIGNED_INT_VEC4 : return "uvec4";
        case GL_BOOL : return	"bool";
        case GL_BOOL_VEC2 : return	"bvec2";
        case GL_BOOL_VEC3 : return	"bvec3";
        case GL_BOOL_VEC4 : return	"bvec4";
        case GL_SAMPLER_2D : return	"sampler2D";
        case GL_SAMPLER_3D : return	"sampler3D";
        case GL_SAMPLER_CUBE : return	"samplerCube";
        case GL_SAMPLER_2D_SHADOW : return	"sampler2DShadow";
        case GL_SAMPLER_2D_ARRAY : return	"sampler2DArray";
        case GL_SAMPLER_2D_ARRAY_SHADOW : return	"sampler2DArrayShadow";
        case GL_SAMPLER_CUBE_SHADOW : return	"samplerCubeShadow";
        case GL_INT_SAMPLER_2D : return	"isampler2D";
        case GL_INT_SAMPLER_3D : return	"isampler3D";
        case GL_INT_SAMPLER_CUBE : return	"isamplerCube";
        case GL_INT_SAMPLER_2D_ARRAY : return	"isampler2DArray";
        case GL_UNSIGNED_INT_SAMPLER_2D : return	"usampler2D";
        case GL_UNSIGNED_INT_SAMPLER_3D : return	"usampler3D";
        case GL_UNSIGNED_INT_SAMPLER_CUBE : return	"usamplerCube";
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY : return	"usampler2DArray";
        default: return "unknown";
    }
}
