#include <glad/glad.h>
#include <string>
#include "Global_H.hpp"
#include "Shader.hpp"

void Shader::UseProgram() const{
    glUseProgram(ProgramID);
}

Shader::Shader()
{
    VertShaderID = glCreateShader(GL_VERTEX_SHADER);
    fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    ProgramID = glCreateProgram();
}

Shader::Shader(std::string _name) : ProgramID(0), VertShaderID(0), fragShaderID(0), name(_name)
{
    VertShaderID = glCreateShader(GL_VERTEX_SHADER);
    fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    ProgramID = glCreateProgram();
    if(!name.empty()){
        Load(name);
    }
        
}

Shader::~Shader()
{
    glDeleteProgram(ProgramID);
}

void Shader::LoadSource()
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
    glShaderSource(VertShaderID, 1, &vertShaderSource, NULL);
   
    const char* fragShaderSource = frag_data.c_str();
    glShaderSource(fragShaderID, 1, &fragShaderSource, NULL);
}

void Shader::Compile(GLuint shader)
{
    glCompileShader(shader);
}

void Shader::Link()
{
    glLinkProgram(ProgramID);
}

void Shader::checkShaderCompileStatus(GLuint shader){
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        ERR("SHADER::COMPILATION_FAILED\n" << infoLog );
        glDeleteShader(shader); // Don't leak the shader.
    }
}

void Shader::checkProgramLinkStatus(GLuint program){
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        ERR("PROGRAM::LINKING_FAILED\n" << infoLog);
        // We don't need the program anymore.
	    glDeleteProgram(program);
    }
}
GLuint Shader::GetProgram() const{
    return ProgramID;
}

GLuint Shader::GetUniformLocation(const char *name)
{
    return glGetUniformLocation(ProgramID, name);
}

void Shader::Load(std::string _name){
    this->name =  _name;
    LoadSource();
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