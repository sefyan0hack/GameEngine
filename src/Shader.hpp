#pragma once
NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END
class Shader
{
    public:
        Shader() ;
        Shader(std::string _name);
        ~Shader();
    public:
        void Load(std::string _name);
        void UseProgram() const ;
        GLuint GetProgram() const;
        GLuint GetUniformLocation(const char* name);
    
    private:
        void LoadSource();
        void Compile(GLuint shader);
        void Link();
        void checkShaderCompileStatus(GLuint shader);
        void checkProgramLinkStatus(GLuint program);
    
    private:
        GLuint VertShaderID;
        GLuint fragShaderID;
        GLuint ProgramID;
    
        std::string name;
};
