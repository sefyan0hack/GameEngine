#pragma once
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
