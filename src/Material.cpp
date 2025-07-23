#include <core/Material.hpp>
#include <core/Log.hpp>
#include <core/Shader.hpp>
#include <core/Texture.hpp>
#include <core/Mesh.hpp>
#include <core/ResourceManager.hpp>
#include <glm/glm.hpp>


auto Material::GLSL_Type_to_string(GLenum type) -> const char*
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


Material::Material(const Shader& vertex, const Shader& fragment)
    : m_Id(gl::CreateProgram())
    , m_Shaders({vertex, fragment})
{

    for(const auto &shader : m_Shaders ){
        gl::AttachShader(m_Id, shader.id());
    }

    Link();
    checkProgramLinkStatus(*this);
    gl::UseProgram(m_Id);
    Info("Attribs count is {}", AttribsCount());
    Info("Uniforms count is {}", UniformCount());
    DumpAttribs();
    DumpUniforms();

    Info("{}", *this);
}
// Material::Material(std::initializer_list<Shader> shaders)
//     : m_Id(gl::CreateProgram())
//     , m_Shaders(std::move(shaders))
// {
//     for(const auto &shader : m_Shaders ){
//         gl::AttachShader(m_Id, shader.id());
//     }
    
//     Link();
//     checkProgramLinkStatus(*this);
//     gl::UseProgram(m_Id);
//     DumpAttribs();
//     DumpUniforms();
// }

Material::Material(const Material& other)
    : m_Id(gl::CreateProgram())
    , m_Shaders(other.m_Shaders)
    , m_Albedo(other.m_Albedo)
{
    for(const auto& shader : m_Shaders ){
        gl::AttachShader(m_Id, shader.id());
    }

    Link();
    checkProgramLinkStatus(*this);
    gl::UseProgram(m_Id);
    DumpAttribs();
    DumpUniforms();
}

Material::Material(Material&& other) noexcept
    : m_Id(std::exchange(other.m_Id, 0))
    , m_Shaders(std::exchange(other.m_Shaders, {}))
    , m_Attribs(std::exchange(other.m_Attribs, {}))
    , m_Uniforms(std::exchange(other.m_Uniforms, {})) // dnt forget  to check if the id are the same in the new Programe
    , m_Albedo(std::exchange(other.m_Albedo, {}))
{
}

Material::~Material()
{
    gl::DeleteProgram(m_Id);
}

auto Material::id() const noexcept -> GLuint
{
    return m_Id;
}

auto Material::Use() const -> void
{
    if(m_Id != 0){
        gl::UseProgram(m_Id); // crache hire even i'm checking the id
        if(m_Albedo)
           m_Albedo->Bind();
    }
}

auto Material::Link() const -> void
{
    gl::LinkProgram(m_Id);
}

auto Material::UniformLocation(const char *name) const -> GLuint
{
    auto it = m_Uniforms.find(name);
    
    if (it != m_Uniforms.end()){
        auto [loc, type, size] = it->second;
        return loc;
    }else{
        Error("the Uniform {} not exist", name);
        return static_cast<GLuint>(-1);
    }
}
auto Material::AttribLocation(const char *name) const -> GLuint
{
    auto it = m_Attribs.find(name);
    
    if (it != m_Attribs.end()){
        return it->second;
    }else{
        Error("the Attrib {} not exist", name);
        return static_cast<GLuint>(-1);
    }
}

auto Material::UniformLocation_Prv(const char *name) const -> GLuint
{
    GLint location = gl::GetUniformLocation(m_Id, name);
    if (location == -1) {
        Error("uniform {} doesn't exist!", name);
    }
    return static_cast<GLuint>(location);
}

auto Material::AttribLocation_Prv(const char *name) const -> GLuint
{
    GLint location = gl::GetAttribLocation(m_Id, name);
    if (location == -1) {
        Error("Attrib {} doesn't exist!", name);
    }
    return static_cast<GLuint>(location);
}

auto Material::checkProgramLinkStatus(const Material& material) -> void
{
    GLuint id = material.id();
    GLint success = GetProgramInfo(id, GL_LINK_STATUS).value_or(0);

    if (!success) {
        GLsizei  infologlength = GetProgramInfo(id, GL_INFO_LOG_LENGTH).value_or(0);
        if(infologlength > 0){
            std::string infoLog(infologlength, '\0');
            gl::GetProgramInfoLog(id, infologlength, nullptr, infoLog.data());
            Error("{}", infoLog);
        }
    }
}
auto Material::UniformCount() const -> GLint
{
    GLint count = GetProgramInfo(m_Id, GL_ACTIVE_UNIFORMS).value_or(-1);

    if(count < 0){
        Info("program id is not a value generated by OpenGL.");
    }

    return count;
}
auto Material::AttribsCount() const -> GLint
{
    GLint count = GetProgramInfo(m_Id, GL_ACTIVE_ATTRIBUTES).value_or(-1);

    if(count < 0){
        Info("error id {}, GetProgramInfo return {}", m_Id, count);
    }

    return count;
}

auto Material::DumpUniforms() -> void
{
    GLint max_len = GetProgramInfo(m_Id, GL_ACTIVE_UNIFORM_MAX_LENGTH).value_or(0);
        
    if(max_len == 0) {
        Info("error id {}, GetProgramInfo return {}", m_Id, max_len);
        return;
    }
    auto count = UniformCount();
    if(count > 0){

        GLsizei len = 0;
        [[maybe_unused]] GLsizei size;
        [[maybe_unused]] GLenum type;

        for(GLint i = 0; i < count; i++){
            std::string Uniform_name(static_cast<std::size_t>(max_len), '\0');
            gl::GetActiveUniform(m_Id, static_cast<GLuint>(i), static_cast<GLsizei>(max_len), &len, &size, &type, Uniform_name.data());
            if(len > 0) Uniform_name.resize(static_cast<std::size_t>(len));

            m_Uniforms[Uniform_name] = std::make_tuple(
                static_cast<GLuint>(UniformLocation_Prv(Uniform_name.c_str())),
                static_cast<GLenum>(type),
                static_cast<GLsizei>(size)
            );
        }
    }
}

auto Material::DumpAttribs() -> void
{
    GLint max_len = GetProgramInfo(m_Id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH).value_or(0);
    if(max_len == 0) {
        Info("max_len is not valid max_len: {}", max_len);
        return;
    }
    
    auto count = AttribsCount();
    if(count > 0){
        GLsizei len = 0;
        [[maybe_unused]] GLenum type;

        for(GLint i = 0; i < count; i++){
            std::string attrib_name(static_cast<std::size_t>(max_len), '\0');
            gl::GetActiveAttrib(m_Id, static_cast<GLuint>(i), max_len, &len, nullptr, &type, attrib_name.data());
            if(len > 0) attrib_name.resize(static_cast<std::size_t>(len));

            m_Attribs[attrib_name] = AttribLocation_Prv(attrib_name.c_str());
        }
    }
}

auto Material::Current_Program() -> GLuint
{
    GLint prog = 0;
    gl::GetIntegerv(GL_CURRENT_PROGRAM, &prog);

    return static_cast<GLuint>(prog);
}

auto Material::Uniforms() const noexcept -> const std::map<std::string, GLSLVar>&
{
    return m_Uniforms;
}
auto Material::Attribs() const noexcept -> const std::map<std::string, GLuint>&
{
    return m_Attribs;
}

auto Material::Shaders() const noexcept -> const std::vector<Shader>&
{
    return m_Shaders;
}

// auto Material::GetShaders() const -> std::vector<GLuint>
// {
//     constexpr GLsizei maxShaderCount = 5;
//     GLsizei count = 0;
//     std::vector<GLuint> shaders(maxShaderCount, 0);
//     gl::GetAttachedShaders(id, maxShaderCount, &count, shaders.data());
//     shaders.resize(count);
//     return shaders;
// }

auto Material::texture() const noexcept-> std::shared_ptr<Texture>
{
    return m_Albedo;
}

auto Material::SetTexture(const std::string &name) -> void
{
    m_Albedo = ResManager.getTexture(name);
}


auto Material::GetProgramInfo(GLint id, GLenum what) -> std::optional<GLint>
{
    auto maxGLint = std::numeric_limits<GLint>::max();
    GLint result = maxGLint;

    gl::GetProgramiv(id, what, &result);

    if(result != maxGLint){
        return result;
    }else{
        Error("{}", glGetError());
        return std::nullopt;
    }
}

///////
auto Material::SetUniform(const std::string& name, const GLint &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        auto [loc, type, size] = it->second;

        Expect(size == 1, "GLSL Uniform size:{} > 1", size);
       
        gl::Uniform1i(static_cast<GLint>(loc), value);
    }else{
        Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const GLfloat &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        auto [loc, type, size] = it->second;
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);
       
        gl::Uniform1f(static_cast<GLint>(loc), value);
    }else{
        Error("the Uniform {} not exist", name);
    }
}
auto Material::SetUniform(const std::string& name, const GLuint &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        auto [loc, type, size] = it->second;
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);
       
        gl::Uniform1ui(static_cast<GLint>(loc), value);
    }else{
        Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const glm::vec2 &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        auto [loc, type, size] = it->second;
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);
       
        gl::Uniform2fv(static_cast<GLint>(loc), 1, &value[0]);
    }else{
        Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const glm::vec3 &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        auto [loc, type, size] = it->second;
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);
       
        gl::Uniform3fv(static_cast<GLint>(loc), 1, &value[0]);
    }else{
        Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const glm::vec4 &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        auto [loc, type, size] = it->second;
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::Uniform4fv(static_cast<GLint>(loc), 1, &value[0]);
    }else{
        Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const glm::mat2 &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        auto [loc, type, size] = it->second;
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);
       
        gl::UniformMatrix2fv(static_cast<GLint>(loc), 1, GL_FALSE, &value[0][0]);
    }else{
        Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const glm::mat3 &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        auto [loc, type, size] = it->second;
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);
       
        gl::UniformMatrix3fv(static_cast<GLint>(loc), 1, GL_FALSE, &value[0][0]);
    }else{
        Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const glm::mat4 &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        auto [loc, type, size] = it->second;
        Expect(size == 1, "GLSL Uniform size:{} > 1", size);

        gl::UniformMatrix4fv(static_cast<GLint>(loc), 1, GL_FALSE, &value[0][0]);
    }else{
        Error("the Uniform {} not exist", name);
    }
}