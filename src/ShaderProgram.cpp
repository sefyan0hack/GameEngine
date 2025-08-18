#include <core/ShaderProgram.hpp>
#include <core/Shader.hpp>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>


ShaderProgram::ShaderProgram(std::shared_ptr<Shader> vertex, std::shared_ptr<Shader> fragment)
    : m_Id(gl::CreateProgram())
    , m_Shaders({vertex, fragment})
{
    for(const auto &shader : m_Shaders ){
        gl::AttachShader(m_Id, shader->id());
    }

    Link();
    checkLinkStatus();
    DumpAttribs();
    DumpUniforms();

    Debug::Print(*this);
}


// ShaderProgram::ShaderProgram(const ShaderProgram& other)
//     : m_Id(gl::CreateProgram())
//     , m_Shaders(other.m_Shaders)
// {
//     for(const auto& shader : m_Shaders ){
//         gl::AttachShader(m_Id, shader->id());
//     }

//     Link();
//     checkLinkStatus();
//     DumpAttribs();
//     DumpUniforms();
// }

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : m_Id(std::exchange(other.m_Id, 0))
    , m_Shaders(std::exchange(other.m_Shaders, {}))
    , m_Attribs(std::exchange(other.m_Attribs, {}))
    , m_Uniforms(std::exchange(other.m_Uniforms, {})) // dnt forget  to check if the id are the same in the new Programe
{
}

ShaderProgram::~ShaderProgram()
{
    gl::DeleteProgram(m_Id);
}

auto ShaderProgram::id() const noexcept -> GLuint
{
    return m_Id;
}

auto ShaderProgram::Use() const -> void
{
    gl::UseProgram(m_Id);
}

auto ShaderProgram::Link() const -> void
{
    gl::LinkProgram(m_Id);
}

auto ShaderProgram::UniformLocation(const char *name) const -> GLuint
{
    try {
        auto it = m_Uniforms.at(name);
        auto [loc, type, size] = it;
        return loc;
    } catch( const std::exception& e){
        throw CException("[what : {} ] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::AttribLocation(const char *name) const -> GLuint
{
    try {
        auto [loc, type, size] = m_Attribs.at(name);
        return loc;
    } catch( const std::exception& e){
        throw CException("[what : {} ] the Attrib `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::UniformLocation_Prv(const char *name) const -> GLuint
{
    GLint location = gl::GetUniformLocation(m_Id, name);
    if (location == -1) {
        Debug::Print("uniform {} doesn't exist!", name);
        // continue;
    }
    return static_cast<GLuint>(location);
}

auto ShaderProgram::AttribLocation_Prv(const char *name) const -> GLuint
{
    GLint location = gl::GetAttribLocation(m_Id, name);
    if (location == -1) {
        throw CException("Attrib {} doesn't exist!", name);
    }
    return static_cast<GLuint>(location);
}

auto ShaderProgram::checkLinkStatus() -> void
{
    GLint success = GetProgramInfo(GL_LINK_STATUS);

    if (!success) {
        GLsizei  infologlength = GetProgramInfo(GL_INFO_LOG_LENGTH);
        if(infologlength > 0){
            std::string infoLog(infologlength, '\0');
            gl::GetProgramInfoLog(m_Id, infologlength, nullptr, infoLog.data());
            throw CException("{}", infoLog);
        }
    }
}
auto ShaderProgram::UniformCount() const -> GLint
{
    GLint count = GetProgramInfo(GL_ACTIVE_UNIFORMS);

    Expect(count >= 0, "GetProgramInfo(GL_ACTIVE_UNIFORMS) failed retuned : {}", count);

    return count;
}
auto ShaderProgram::AttribsCount() const -> GLint
{
    GLint count = GetProgramInfo(GL_ACTIVE_ATTRIBUTES);

    Expect(count >= 0, "GetProgramInfo(GL_ACTIVE_ATTRIBUTES) failed retuned : {}", count);

    return count;
}

auto ShaderProgram::DumpUniforms() -> void
{
    auto count = UniformCount();

    // properties we want: name length, type, array size, block index, location
    constexpr GLenum props[] = {
        GL_NAME_LENGTH,
        GL_TYPE,
        GL_ARRAY_SIZE,
        GL_BLOCK_INDEX,
        GL_LOCATION
    };

    constexpr GLint propCount = (GLint)(sizeof(props)/sizeof(props[0]));

    for (GLint idx = 0; idx < count; ++idx) {
        GLint results[propCount];
        gl::GetProgramResourceiv(m_Id, GL_UNIFORM, static_cast<GLuint>(idx), propCount, props, propCount, nullptr, results);

        GLint nameLen    = results[0]; // includes NULL
        GLenum type      = static_cast<GLenum>(results[1]);
        GLint arraySize  = results[2];
        GLint blockIndex = results[3];
        GLint location   = results[4];

        // skip uniforms that are inside uniform blocks (optional)
        if (blockIndex != -1) continue;

        std::string name(static_cast<std::size_t>(std::max(1, nameLen)), '\0');
        if (nameLen > 0) {
            gl::GetProgramResourceName(m_Id, GL_UNIFORM, static_cast<GLuint>(idx), nameLen, nullptr, &name[0]);
            // remove terminating null
            if (!name.empty() && name.back() == '\0') name.pop_back();
        }

        if (location == -1) throw CException("loaction is -1 on uniform : {}", name);

        m_Uniforms[name] = std::make_tuple(
            static_cast<GLuint>(location),
            static_cast<GLenum>(type),
            static_cast<GLsizei>(arraySize)
        );
    }
}

auto ShaderProgram::DumpAttribs() -> void
{
    GLint activeInputs = 0;
    gl::GetProgramInterfaceiv(m_Id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &activeInputs);
    if (activeInputs <= 0) {
        Debug::Print("no active attributes for program {}\n", m_Id);
        return;
    }

    constexpr GLenum props[] = {
        GL_NAME_LENGTH,
        GL_TYPE,
        GL_ARRAY_SIZE,
        GL_LOCATION
    };
    constexpr int propCount = sizeof(props)/sizeof(props[0]);

    for (GLint idx = 0; idx < activeInputs; ++idx) {
        GLint results[propCount];
        gl::GetProgramResourceiv(m_Id, GL_PROGRAM_INPUT, static_cast<GLuint>(idx),
                                 propCount, props, propCount, nullptr, results);

        GLint nameLen   = results[0]; // includes NULL
        GLenum type     = static_cast<GLenum>(results[1]);
        GLint arraySize = results[2];
        GLint location  = results[3];

        // get the attribute name
        std::string name(std::max(1, nameLen), '\0');
        if (nameLen > 0) {
            gl::GetProgramResourceName(m_Id, GL_PROGRAM_INPUT, static_cast<GLuint>(idx),
                                       nameLen, nullptr, &name[0]);
            if (!name.empty() && name.back() == '\0') name.pop_back();
        }

        if(name.starts_with("gl_")) continue;

        if (location == -1) throw CException("location is -1 for in attribute: {}", name);

        m_Attribs[name] = std::make_tuple(
            static_cast<GLuint>(location),
            static_cast<GLenum>(type),
            static_cast<GLsizei>(arraySize)
        );
    }
}

auto ShaderProgram::Current_Program() -> GLuint
{
    GLint prog = 0;
    gl::GetIntegerv(GL_CURRENT_PROGRAM, &prog);

    return static_cast<GLuint>(prog);
}

auto ShaderProgram::Uniforms() const noexcept -> const std::map<std::string, GLSLVar>&
{
    return m_Uniforms;
}

auto ShaderProgram::Attribs() const noexcept -> const std::map<std::string, GLSLVar>&
{
    return m_Attribs;
}

auto ShaderProgram::GetProgramInfo(GLenum what) const -> GLint
{
    constexpr auto INVALID = std::numeric_limits<GLint>::max();

    GLint result = INVALID;

    gl::GetProgramiv(m_Id, what, &result);

    if(result != INVALID)
        return result;
    else
        throw CException("{}", GL_ERR_to_string(glGetError()));
}

///////
auto ShaderProgram::SetUniform(const std::string& name, const GLint &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform1i(static_cast<GLint>(loc), value);
    } catch(const std::exception& e) {
        throw CException("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::SetUniform(const std::string& name, const GLfloat &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform1f(static_cast<GLint>(loc), value);
    } catch(const std::exception& e) {
        throw CException("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}
auto ShaderProgram::SetUniform(const std::string& name, const GLuint &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform1ui(static_cast<GLint>(loc), value);
    } catch(const std::exception& e) {
        throw CException("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::SetUniform(const std::string& name, const glm::vec2 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform2fv(static_cast<GLint>(loc), 1, &value[0]);
    } catch(const std::exception& e) {
        throw CException("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::SetUniform(const std::string& name, const glm::vec3 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform3fv(static_cast<GLint>(loc), 1, &value[0]);
    } catch(const std::exception& e) {
        throw CException("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::SetUniform(const std::string& name, const glm::vec4 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform4fv(static_cast<GLint>(loc), 1, &value[0]);
    } catch(const std::exception& e) {
        throw CException("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::SetUniform(const std::string& name, const glm::mat2 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::UniformMatrix2fv(static_cast<GLint>(loc), 1, GL_FALSE, &value[0][0]);
    } catch(const std::exception& e) {
        throw CException("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::SetUniform(const std::string& name, const glm::mat3 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::UniformMatrix3fv(static_cast<GLint>(loc), 1, GL_FALSE, &value[0][0]);
    } catch(const std::exception& e) {
        throw CException("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}

auto ShaderProgram::SetUniform(const std::string& name, const glm::mat4 &value) const -> void
{
    try {
        auto [loc, type, size] = m_Uniforms.at(name);
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::UniformMatrix4fv(static_cast<GLint>(loc), 1, GL_FALSE, &value[0][0]);
    } catch(const std::exception& e) {
        throw CException("[what: {}] the Uniform `{}` not exist", e.what(), name);
    }
}


auto ShaderProgram::GLSL_Type_to_string(GLenum type) -> const char*
{
    switch (type)
    {
        case GL_FLOAT: return "GL_FLOAT";
        case GL_FLOAT_VEC2: return "GL_FLOAT_VEC2";
        case GL_FLOAT_VEC3: return "GL_FLOAT_VEC3";
        case GL_FLOAT_VEC4: return "GL_FLOAT_VEC4";
        case GL_DOUBLE: return "GL_DOUBLE";
        case GL_INT: return "GL_INT";
        case GL_INT_VEC2: return "GL_INT_VEC2";
        case GL_INT_VEC3: return "GL_INT_VEC3";
        case GL_INT_VEC4: return "GL_INT_VEC4";
        case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
        case GL_UNSIGNED_INT_VEC2: return "GL_UNSIGNED_INT_VEC2";
        case GL_UNSIGNED_INT_VEC3: return "GL_UNSIGNED_INT_VEC3";
        case GL_UNSIGNED_INT_VEC4: return "GL_UNSIGNED_INT_VEC4";
        case GL_BOOL: return "GL_BOOL";
        case GL_BOOL_VEC2: return "GL_BOOL_VEC2";
        case GL_BOOL_VEC3: return "GL_BOOL_VEC3";
        case GL_BOOL_VEC4: return "GL_BOOL_VEC4";
        case GL_FLOAT_MAT2: return "GL_FLOAT_MAT2";
        case GL_FLOAT_MAT3: return "GL_FLOAT_MAT3";
        case GL_FLOAT_MAT4: return "GL_FLOAT_MAT4";
        case GL_FLOAT_MAT2x3: return "GL_FLOAT_MAT2x3";
        case GL_FLOAT_MAT2x4: return "GL_FLOAT_MAT2x4";
        case GL_FLOAT_MAT3x2: return "GL_FLOAT_MAT3x2";
        case GL_FLOAT_MAT3x4: return "GL_FLOAT_MAT3x4";
        case GL_FLOAT_MAT4x2: return "GL_FLOAT_MAT4x2";
        case GL_FLOAT_MAT4x3: return "GL_FLOAT_MAT4x3";
        case GL_SAMPLER_1D: return "GL_SAMPLER_1D";
        case GL_SAMPLER_2D: return "GL_SAMPLER_2D";
        case GL_SAMPLER_3D: return "GL_SAMPLER_3D";
        case GL_SAMPLER_CUBE: return "GL_SAMPLER_CUBE";
        case GL_SAMPLER_1D_SHADOW: return "GL_SAMPLER_1D_SHADOW";
        case GL_SAMPLER_2D_SHADOW: return "GL_SAMPLER_2D_SHADOW";
        case GL_SAMPLER_1D_ARRAY: return "GL_SAMPLER_1D_ARRAY";
        case GL_SAMPLER_2D_ARRAY: return "GL_SAMPLER_2D_ARRAY";
        case GL_SAMPLER_1D_ARRAY_SHADOW: return "GL_SAMPLER_1D_ARRAY_SHADOW";
        case GL_SAMPLER_2D_ARRAY_SHADOW: return "GL_SAMPLER_2D_ARRAY_SHADOW";
        case GL_SAMPLER_2D_MULTISAMPLE: return "GL_SAMPLER_2D_MULTISAMPLE";
        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_SAMPLER_2D_MULTISAMPLE_ARRAY";
        case GL_SAMPLER_CUBE_SHADOW: return "GL_SAMPLER_CUBE_SHADOW";
        case GL_SAMPLER_BUFFER: return "GL_SAMPLER_BUFFER";
        case GL_SAMPLER_2D_RECT: return "GL_SAMPLER_2D_RECT";
        case GL_SAMPLER_2D_RECT_SHADOW: return "GL_SAMPLER_2D_RECT_SHADOW";
        case GL_INT_SAMPLER_1D: return "GL_INT_SAMPLER_1D";
        case GL_INT_SAMPLER_2D: return "GL_INT_SAMPLER_2D";
        case GL_INT_SAMPLER_3D: return "GL_INT_SAMPLER_3D";
        case GL_INT_SAMPLER_CUBE: return "GL_INT_SAMPLER_CUBE";
        case GL_INT_SAMPLER_1D_ARRAY: return "GL_INT_SAMPLER_1D_ARRAY";
        case GL_INT_SAMPLER_2D_ARRAY: return "GL_INT_SAMPLER_2D_ARRAY";
        case GL_INT_SAMPLER_2D_MULTISAMPLE: return "GL_INT_SAMPLER_2D_MULTISAMPLE";
        case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
        case GL_INT_SAMPLER_BUFFER: return "GL_INT_SAMPLER_BUFFER";
        case GL_INT_SAMPLER_2D_RECT: return "GL_INT_SAMPLER_2D_RECT";
        case GL_UNSIGNED_INT_SAMPLER_1D: return "GL_UNSIGNED_INT_SAMPLER_1D";
        case GL_UNSIGNED_INT_SAMPLER_2D: return "GL_UNSIGNED_INT_SAMPLER_2D";
        case GL_UNSIGNED_INT_SAMPLER_3D: return "GL_UNSIGNED_INT_SAMPLER_3D";
        case GL_UNSIGNED_INT_SAMPLER_CUBE: return "GL_UNSIGNED_INT_SAMPLER_CUBE";
        case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY";
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY";
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE";
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
        case GL_UNSIGNED_INT_SAMPLER_BUFFER: return "GL_UNSIGNED_INT_SAMPLER_BUFFER";
        case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return "GL_UNSIGNED_INT_SAMPLER_2D_RECT";
        default: return "unknown";
    }
}