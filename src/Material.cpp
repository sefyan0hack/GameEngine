#include <core/Material.hpp>
#include <core/Shader.hpp>
#include <glm/glm.hpp>


Material::Material(Shader& vertex, Shader& fragment)
    : id(glCreateProgram())
{
    Shaders.vertex = &vertex;
    Shaders.fragment = &fragment;
    
    glAttachShader(id, Shaders.vertex->Getid());
    glAttachShader(id, Shaders.fragment->Getid());

    Link();
    checkProgramLinkStatus();

}

Material::~Material()
{
    glUseProgram(0);
    glDeleteProgram(id);
}

auto Material::Getid() const -> unsigned int
{
    return id;
}

auto Material::Use() const -> void
{
    glUseProgram(id);
}

auto Material::Link() const -> void
{
    glLinkProgram(id);
}

auto Material::GetUniformLocation(const char *name) const -> unsigned int
{
    Use();
    // If not cached, query the location and cache it
    unsigned int location = glGetUniformLocation(id, name);
    if (location == -1) {
        ERR("Warning: uniform '" << name << "' doesn't exist!");
    }
    return location;
}

auto Material::checkProgramLinkStatus() const -> void
{
    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        ERR("PROGRAM::LINKING_FAILED\n" << infoLog);
        // We don't need the program anymore.
	    glDeleteProgram(id);
    }
}
///////
template<>
auto Material::SetUniform<GLint>(const std::string& name, const GLint &value) const -> void
{
    glUniform1i(GetUniformLocation(name.c_str()), value);
}
template<>
auto Material::SetUniform<GLfloat>(const std::string& name, const GLfloat &value) const -> void
{
    glUniform1f(GetUniformLocation(name.c_str()), value);
}
template<>
auto Material::SetUniform<GLuint>(const std::string& name, const GLuint &value) const -> void
{
    glUniform1ui(GetUniformLocation(name.c_str()), value);
}
//
template<>
auto Material::SetUniform<glm::mat2>(const std::string& name, const glm::mat2 &value) const -> void
{
    glUniformMatrix2fv(GetUniformLocation(name.c_str()), 1, GL_FALSE, &value[0][0]);
}
template<>
auto Material::SetUniform<glm::mat3>(const std::string& name, const glm::mat3 &value) const -> void
{
    glUniformMatrix3fv(GetUniformLocation(name.c_str()), 1, GL_FALSE, &value[0][0]);
}
template<>
auto Material::SetUniform<glm::mat4>(const std::string& name, const glm::mat4 &value) const -> void
{
    glUniformMatrix4fv(GetUniformLocation(name.c_str()), 1, GL_FALSE, &value[0][0]);
}
///////

template<>
auto Material::SetUniform<GLint>(const std::string& name, const GLint &value1, const GLint &value2) const -> void
{
    glUniform2i(GetUniformLocation(name.c_str()), value1, value2);
}
template<>
auto Material::SetUniform<GLfloat>(const std::string& name, const GLfloat &value1, const GLfloat &value2) const -> void
{
    glUniform2f(GetUniformLocation(name.c_str()), value1, value2);
}
template<>
auto Material::SetUniform<GLuint>(const std::string& name, const GLuint &value1, const GLuint &value2) const -> void
{
    glUniform2ui(GetUniformLocation(name.c_str()), value1, value2);
}
///////

template<>
auto Material::SetUniform<GLint>(const std::string& name, const GLint &value1, const GLint &value2, const GLint &value3) const -> void
{
    glUniform3i(GetUniformLocation(name.c_str()), value1, value2, value3);
}
template<>
auto Material::SetUniform<GLfloat>(const std::string& name, const GLfloat &value1, const GLfloat &value2, const GLfloat &value3) const -> void
{
    glUniform3f(GetUniformLocation(name.c_str()), value1, value2, value3);
}
template<>
auto Material::SetUniform<GLuint>(const std::string& name, const GLuint &value1, const GLuint &value2, const GLuint &value3) const -> void
{
    glUniform3ui(GetUniformLocation(name.c_str()), value1, value2, value3);
}
///////

template<>
auto Material::SetUniform<GLint>(const std::string& name, const GLint &value1, const GLint &value2, const GLint &value3, const GLint &value4) const -> void
{
    glUniform4i(GetUniformLocation(name.c_str()), value1, value2, value3, value4);
}
template<>
auto Material::SetUniform<GLfloat>(const std::string& name, const GLfloat &value1, const GLfloat &value2, const GLfloat &value3, const GLfloat &value4) const -> void
{
    glUniform4f(GetUniformLocation(name.c_str()), value1, value2, value3, value4);
}
template<>
auto Material::SetUniform<GLuint>(const std::string& name, const GLuint &value1, const GLuint &value2, const GLuint &value3, const GLuint &value4) const -> void
{
    glUniform4ui(GetUniformLocation(name.c_str()), value1, value2, value3, value4);
}