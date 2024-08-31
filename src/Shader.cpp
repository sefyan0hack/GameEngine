#include <glad/glad.h>
#include <string>
#include "Global_H.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
void Shader::Use() const{
    glUseProgram(ProgramID);
}

Shader::Shader()
: ProgramID(glCreateProgram()), name("") {}

Shader::Shader(const std::string &_name) 
: ProgramID(glCreateProgram()), name(_name)
{
    if(!name.empty()){
        Load(name);
    }
    else{
        throw "no shader name brovided";
    }
}

Shader::~Shader()
{
    glDeleteProgram(ProgramID);
}

void Shader::LoadSource(unsigned int VertShader, unsigned int fragShader)
{
    if(name.empty()) ERR("name is empty");

    std::string full_name_frag_file(name);
    full_name_frag_file += ".frag";

    std::string full_name_vert_file(name);
    full_name_vert_file += ".vert";

    FILE *frag_file;
    FILE *vert_file;

    frag_file = fopen64(full_name_frag_file.c_str(), "r+b");
    vert_file = fopen64(full_name_vert_file.c_str(), "r+b");
    
    if(frag_file == nullptr || vert_file == nullptr){
        ERR("Open " << full_name_frag_file << " OR " << full_name_vert_file <<" Failed. code: " << errno);
    }

    LOG("[+] Loding " << full_name_frag_file );
    LOG("[+] Loding " << full_name_vert_file );

    std::string frag_data, vert_data ;
    char  byte;
    int i = 0;
    while (fread(&byte, sizeof(char), 1, vert_file))
    {
        vert_data.resize(vert_data.size() + 1);
        vert_data.at(i++) = byte;
    }

    byte = '\n';
    i = 0;
    while (fread(&byte, sizeof(char), 1, frag_file))
    {
        frag_data.resize(frag_data.size() + 1);
        frag_data.at(i++) = byte;
    }

    if(fclose(frag_file) != 0 || fclose(vert_file) != 0){
        ERR("Closing Files");
    }

    const char* vertShaderSource = vert_data.c_str();
    glShaderSource(VertShader, 1, &vertShaderSource, NULL);
   
    const char* fragShaderSource = frag_data.c_str();
    glShaderSource(fragShader, 1, &fragShaderSource, NULL);
}

void Shader::Compile(GLuint shader)
{
    glCompileShader(shader);
}

void Shader::Link()
{
    glLinkProgram(ProgramID);
}

void Shader::checkShaderCompileStatus(const GLuint &shader){
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        ERR("SHADER::COMPILATION_FAILED\n" << infoLog );
        glDeleteShader(shader); // Don't leak the shader.
    }
}

void Shader::checkProgramLinkStatus(const GLuint &program){
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        ERR("PROGRAM::LINKING_FAILED\n" << infoLog);
        // We don't need the program anymore.
	    glDeleteProgram(program);
    }
}
GLuint Shader::GetProgram() const{
    return ProgramID;
}

GLuint Shader::GetUniformLocation(const char *name) const
{
    auto it = UniformLocations.find(name);
    if (it != UniformLocations.end()) {
        return it->second;
    }
    
    Use();
    // If not cached, query the location and cache it
    GLint location = glGetUniformLocation(ProgramID, name);
    if (location == -1) {
        ERR("Warning: uniform '" << name << "' doesn't exist!");
    }

    UniformLocations[name] = location;
    return location;
}

void Shader::Load(const std::string &_name){
    this->name =  _name;
    unsigned int VertShaderID = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    LoadSource(VertShaderID, fragShaderID);
    Compile(VertShaderID);
    Shader::checkShaderCompileStatus(VertShaderID);

    Compile(fragShaderID);
    Shader::checkShaderCompileStatus(fragShaderID);

    glAttachShader(ProgramID, VertShaderID);
    glAttachShader(ProgramID, fragShaderID);
    Link();
    Shader::checkProgramLinkStatus(ProgramID);

    glDeleteShader(VertShaderID);
    glDeleteShader(fragShaderID);
}
///////
template<>
void Shader::SetUniform<GLint>(const std::string& name, const GLint &value) const {
    glUniform1i(GetUniformLocation(name.c_str()), value);
}
template<>
void Shader::SetUniform<GLfloat>(const std::string& name, const GLfloat &value) const {
    glUniform1f(GetUniformLocation(name.c_str()), value);
}
template<>
void Shader::SetUniform<GLuint>(const std::string& name, const GLuint &value) const {
    glUniform1ui(GetUniformLocation(name.c_str()), value);
}
//
template<>
void Shader::SetUniform<glm::mat2>(const std::string& name, const glm::mat2 &value) const {
    glUniformMatrix2fv(GetUniformLocation(name.c_str()), 1, GL_FALSE, &value[0][0]);
}
template<>
void Shader::SetUniform<glm::mat3>(const std::string& name, const glm::mat3 &value) const {
    glUniformMatrix3fv(GetUniformLocation(name.c_str()), 1, GL_FALSE, &value[0][0]);
}
template<>
void Shader::SetUniform<glm::mat4>(const std::string& name, const glm::mat4 &value) const {
    glUniformMatrix4fv(GetUniformLocation(name.c_str()), 1, GL_FALSE, &value[0][0]);
}
///////

template<>
void Shader::SetUniform<GLint>(const std::string& name, const GLint &value1, const GLint &value2) const {
    glUniform2i(GetUniformLocation(name.c_str()), value1, value2);
}
template<>
void Shader::SetUniform<GLfloat>(const std::string& name, const GLfloat &value1, const GLfloat &value2) const {
    glUniform2f(GetUniformLocation(name.c_str()), value1, value2);
}
template<>
void Shader::SetUniform<GLuint>(const std::string& name, const GLuint &value1, const GLuint &value2) const {
    glUniform2ui(GetUniformLocation(name.c_str()), value1, value2);
}
///////

template<>
void Shader::SetUniform<GLint>(const std::string& name, const GLint &value1, const GLint &value2, const GLint &value3) const {
    glUniform3i(GetUniformLocation(name.c_str()), value1, value2, value3);
}
template<>
void Shader::SetUniform<GLfloat>(const std::string& name, const GLfloat &value1, const GLfloat &value2, const GLfloat &value3) const {
    glUniform3f(GetUniformLocation(name.c_str()), value1, value2, value3);
}
template<>
void Shader::SetUniform<GLuint>(const std::string& name, const GLuint &value1, const GLuint &value2, const GLuint &value3) const {
    glUniform3ui(GetUniformLocation(name.c_str()), value1, value2, value3);
}
///////

template<>
void Shader::SetUniform<GLint>(const std::string& name, const GLint &value1, const GLint &value2, const GLint &value3, const GLint &value4) const {
    glUniform4i(GetUniformLocation(name.c_str()), value1, value2, value3, value4);
}
template<>
void Shader::SetUniform<GLfloat>(const std::string& name, const GLfloat &value1, const GLfloat &value2, const GLfloat &value3, const GLfloat &value4) const {
    glUniform4f(GetUniformLocation(name.c_str()), value1, value2, value3, value4);
}
template<>
void Shader::SetUniform<GLuint>(const std::string& name, const GLuint &value1, const GLuint &value2, const GLuint &value3, const GLuint &value4) const {
    glUniform4ui(GetUniformLocation(name.c_str()), value1, value2, value3, value4);
}