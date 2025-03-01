#include <core/Material.hpp>
#include <core/Log.hpp>
#include <core/Shader.hpp>
#include <core/Texture.hpp>
#include <core/Mesh.hpp>
#include <glm/glm.hpp>
#include <ranges>


Material::Material(const Shader& vertex, const Shader& fragment)
    : m_Id(glCreateProgram())
    , m_Shaders({vertex, fragment})
    , m_Albedo(nullptr)
    , m_Previd(Current_Program())
{

    for(const auto &shader : m_Shaders ){
        glAttachShader(m_Id, shader.id());
    }

    Link();
    checkProgramLinkStatus();
    Log::Info("Attribs count is {}", AttribsCount());
    Log::Info("Uniforms count is {}", UniformCount());
    DumpAttribs();
    DumpUniforms();
    Log::Info("{}", *this);
}
Material::Material(std::initializer_list<Shader> shaders)
    : m_Id(glCreateProgram())
    , m_Shaders(std::move(shaders))
    , m_Albedo(nullptr)
    , m_Previd(Current_Program())
{
    for(const auto &shader : m_Shaders ){
        glAttachShader(m_Id, shader.id());
    }
    
    Link();
    checkProgramLinkStatus();
    DumpAttribs();
    DumpUniforms();
    Log::Info("{}", *this);
}

Material::Material(const Material& other)
    : m_Id(glCreateProgram())
    , m_Shaders(other.m_Shaders)
    , m_Albedo(other.m_Albedo)
    , m_Previd(Current_Program())
{
    for(const auto& shader : m_Shaders ){
        glAttachShader(m_Id, shader.id());
    }

    Link();
    checkProgramLinkStatus();
    DumpAttribs();
    DumpUniforms();
    Log::Info("{}", *this);
}
Material::Material(Material&& other)
    : m_Id(std::move(other.m_Id))
    , m_Shaders(std::move(other.m_Shaders))
    , m_Attribs(std::move(other.m_Attribs))
    , m_Uniforms(std::move(other.m_Uniforms)) // dnt forget  to check if the id are the same in the new Programe
    , m_Albedo(std::move(other.m_Albedo))
    , m_Previd(Current_Program())

{
    other.m_Id = 0;
    other.m_Shaders.clear();
    other.m_Attribs.clear();
    other.m_Uniforms.clear();
}

Material::~Material()
{
    if(glIsProgram(m_Id) == GL_TRUE) glDeleteProgram(m_Id);
    glUseProgram(m_Previd);
}

auto Material::id() const noexcept -> GLuint
{
    return m_Id;
}

auto Material::Use() const -> void
{
    m_Previd = Current_Program();
    if(m_Id != 0 && glIsProgram(m_Id) == GL_TRUE){
        glUseProgram(m_Id); // crache hire even i'm checking the id
        if(m_Albedo)
           m_Albedo->Bind();
    }
}

auto Material::UnUse() const -> void
{
    glUseProgram(m_Previd);
    if(m_Albedo)
        m_Albedo->UnBind();
}

auto Material::Link() const -> void
{
    glLinkProgram(m_Id);
}

auto Material::UniformLocation(const char *name) const -> GLuint
{
    auto it = m_Uniforms.find(name);
    
    if (it != m_Uniforms.end()){
        return it->second;
    }else{
        Log::Error("the Uniform {} not exist", name);
        return static_cast<GLuint>(-1);
    }
}
auto Material::AttribLocation(const char *name) const -> GLuint
{
    auto it = m_Attribs.find(name);
    
    if (it != m_Attribs.end()){
        return it->second;
    }else{
        Log::Error("the Attrib {} not exist", name);
        return static_cast<GLuint>(-1);
    }
}

auto Material::UniformLocation_Prv(const char *name) const -> GLuint
{
    Use();
    GLint location = glGetUniformLocation(m_Id, name);
    if (location == -1) {
        Log::Error("uniform {} doesn't exist!", name);
    }
    return static_cast<GLuint>(location);
}

auto Material::AttribLocation_Prv(const char *name) const -> GLuint
{
    Use();
    GLint location = glGetAttribLocation(m_Id, name);
    if (location == -1) {
        Log::Error("Attrib {} doesn't exist!", name);
    }
    return static_cast<GLuint>(location);
}

auto Material::checkProgramLinkStatus() const -> void
{
    GLint success;
    glGetProgramiv(m_Id, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_Id, 512, NULL, infoLog);
	    // glDeleteProgram(id);
        Log::Error("PROGRAM LINKING_FAILED {}", infoLog);
    }
}
auto Material::UniformCount() const -> GLint
{
    Log::Expect(glIsProgram(m_Id) == GL_TRUE , "program id {} is not a value generated by OpenGL", (GLuint)m_Id);
    GLint count;
    glGetProgramiv(m_Id, GL_ACTIVE_UNIFORMS, &count);

    if(count <= 0){
        Log::Error("program id is not a value generated by OpenGL.");
    }

    return count;
}
auto Material::AttribsCount() const -> GLint
{
    Log::Expect(glIsProgram(m_Id) == GL_TRUE, "program id {} is not a value generated by OpenGL", (GLuint)m_Id);

    GLint count;
    glGetProgramiv(m_Id, GL_ACTIVE_ATTRIBUTES, &count);

    if(count <= 0){
        Log::Error("program id is not a value generated by OpenGL.");
    }

    return count;
}

auto Material::DumpUniforms() -> void
{
    GLint max_len = 0;
    glGetProgramiv(m_Id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_len);
    
    Log::Expect(max_len > 0, "max_len  is not  valid max_len: {}", max_len);

    auto count = UniformCount();
    if(count > 0){

        GLsizei len;
        [[maybe_unused]] GLsizei count_out;
        [[maybe_unused]] GLenum type;

        for(GLint i = 0; i < count; i++){
            std::string Uniform_name(static_cast<size_t>(max_len), '\0');
            glGetActiveUniform(m_Id, i, static_cast<GLsizei>(max_len), &len, &count_out, &type, Uniform_name.data());
            Uniform_name.resize(static_cast<size_t>(len));

            m_Uniforms[Uniform_name] =  UniformLocation_Prv(Uniform_name.c_str());
        }
    }
}

auto Material::DumpAttribs() -> void
{
    GLint max_len = 0;
    glGetProgramiv(m_Id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_len);
    
    Log::Expect(max_len > 0, "max_len  is not  valid max_len: {}", max_len);

    auto count = AttribsCount();
    if(count > 0){
        GLsizei len;
        [[maybe_unused]] GLenum type;

        for(GLint i = 0; i < count; i++){
            std::string attrib_name(static_cast<size_t>(max_len), '\0');
            glGetActiveAttrib(m_Id, i, max_len, &len, nullptr, &type, attrib_name.data());
            attrib_name.resize(static_cast<size_t>(len));

            m_Attribs[attrib_name] = AttribLocation_Prv(attrib_name.c_str());
        }
    }
}

auto Material::Current_Program() -> GLuint{
    GLint prog = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);

    return prog;
}

auto Material::Uniforms() const noexcept -> const std::map<std::string, GLuint>&
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
//     glGetAttachedShaders(id, maxShaderCount, &count, shaders.data());
//     shaders.resize(count);
//     return shaders;
// }

auto Material::texture() const noexcept-> std::shared_ptr<Texture>
{
    return m_Albedo;
}

auto Material::SetTexture(const std::string &name) -> void
{
    m_Albedo = std::make_shared<Texture2D>(name);
}

auto Material::SetTexture(const std::vector<std::string> faces) -> void
{
    m_Albedo = std::make_shared<TextureCubeMap>(faces);
}

// auto Material::EnableAttribs() const -> void
// {
//     for(const auto& [aName, aIndex] : Attribs | std::views::reverse){
//         glEnableVertexAttribArray(aIndex);
//     }
// }

// auto Material::DisableAttribs() const -> void
// {
//     for(const auto& [aName, aIndex] : Attribs | std::views::reverse){
//         glDisableVertexAttribArray(aIndex);
//     }
// }

///////
auto Material::SetUniform(const std::string& name, const GLint &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform1i(it->second, value);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const GLfloat &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform1f(it->second, value);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}
auto Material::SetUniform(const std::string& name, const GLuint &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform1ui(it->second, value);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const glm::vec2 &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform2fv(it->second, 1, &value[0]);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const glm::vec3 &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform3fv(it->second, 1, &value[0]);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const glm::mat2 &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniformMatrix2fv(it->second, 1, GL_FALSE, &value[0][0]);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const glm::mat3 &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniformMatrix3fv(it->second, 1, GL_FALSE, &value[0][0]);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const glm::mat4 &value) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniformMatrix4fv(it->second, 1, GL_FALSE, &value[0][0]);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}
///////

auto Material::SetUniform(const std::string& name, const GLint &value1, const GLint &value2) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform2i(it->second, value1, value2);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const GLfloat &value1, const GLfloat &value2) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform2f(it->second, value1, value2);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const GLuint &value1, const GLuint &value2) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform2ui(it->second, value1, value2);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}
///////

auto Material::SetUniform(const std::string& name, const GLint &value1, const GLint &value2, const GLint &value3) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform3i(it->second, value1, value2, value3);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const GLfloat &value1, const GLfloat &value2, const GLfloat &value3) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform3f(it->second, value1, value2, value3);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const GLuint &value1, const GLuint &value2, const GLuint &value3) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform3ui(it->second, value1, value2, value3);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}
///////

auto Material::SetUniform(const std::string& name, const GLint &value1, const GLint &value2, const GLint &value3, const GLint &value4) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform4i(it->second, value1, value2, value3, value4);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const GLfloat &value1, const GLfloat &value2, const GLfloat &value3, const GLfloat &value4) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform4f(it->second, value1, value2, value3, value4);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}

auto Material::SetUniform(const std::string& name, const GLuint &value1, const GLuint &value2, const GLuint &value3, const GLuint &value4) const -> void
{
    auto it = m_Uniforms.find(name);
    if (it != m_Uniforms.end()){
        glUniform4ui(it->second, value1, value2, value3, value4);
    }else{
        Log::Error("the Uniform {} not exist", name);
    }
}