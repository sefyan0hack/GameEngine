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
        explicit Shader(const std::string &_name);
        ~Shader();
    public:
        void Load(const std::string &_name);
        void Use() const ;
        GLuint GetProgram() const;
        GLuint GetUniformLocation(const char* name) const;
        template<class T>
        void SetUniform(const std::string &name, const T &value) const;

        template<class T>
        void SetUniform(const std::string &name, const T& value1, const T& value2) const;

        template<class T>
        void SetUniform(const std::string &name, const T& value1, const T& value2, const T& value3) const;

        template<class T>
        void SetUniform(const std::string &name, const T& value1, const T& value2, const T& value3, const T& value4) const;

    private:
        void LoadSource(unsigned int VertShader, unsigned int fragShader);
        void Compile(GLuint shader);
        void Link();
        void checkShaderCompileStatus(const GLuint &shader);
        void checkProgramLinkStatus(const GLuint &program);
    
    private:
        const GLuint ProgramID;
        std::string name;
        mutable std::unordered_map<std::string, GLuint> UniformLocations;
};
