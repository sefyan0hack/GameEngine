#pragma once
#include "Global_H.hpp"
#include <unordered_map>
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
        void Use() const ;
        GLuint GetProgram() const;
        GLuint GetUniformLocation(const char* name);
        template<class T>
        void SetUniform(const std::string &name, T value);

        template<class T>
        void SetUniform(const std::string &name, T value1, T value2);

        template<class T>
        void SetUniform(const std::string &name, T value1, T value2, T value3);

        template<class T>
        void SetUniform(const std::string &name, T value1, T value2, T value3, T value4);

    private:
        void LoadSource(unsigned int VertShader, unsigned int fragShader);
        void Compile(GLuint shader);
        void Link();
        void checkShaderCompileStatus(GLuint shader);
        void checkProgramLinkStatus(GLuint program);
    
    private:
        const GLuint ProgramID;
        std::string name;
        std::unordered_map<std::string, GLuint> UniformLocations;
};
