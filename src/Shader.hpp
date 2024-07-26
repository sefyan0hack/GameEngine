#pragma once
class Shader
{
public:
    Shader(std::string _name);
    ~Shader();
public:
    void UseProgram() const ;

private:
    void LoadSource();
    void Compile(GLuint shader);
    void Link();
    void checkShaderCompileStatus(GLuint shader);
    void checkProgramLinkStatus(GLuint program);

private:
    GLuint ProgramID;
    GLuint VertShaderID;
    GLuint fragShaderID;

    std::string name;
};
