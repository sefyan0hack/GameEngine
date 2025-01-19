#pragma once
#include <core/Global_H.hpp>
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
        auto Load(const std::string &_name)             -> void ;
        auto Use() const                                -> void ;
        auto GetProgram() const                         -> GLuint ;
        auto GetUniformLocation(const char* name) const -> GLuint ;
        template<class T>
        auto SetUniform(const std::string &name, const T &value) const -> void;

        template<class T>
        auto SetUniform(const std::string &name, const T& value1, const T& value2) const -> void;

        template<class T>
        auto SetUniform(const std::string &name, const T& value1, const T& value2, const T& value3) const -> void;

        template<class T>
        auto SetUniform(const std::string &name, const T& value1, const T& value2, const T& value3, const T& value4) const -> void;

    private:
        auto LoadSource(unsigned int VertShader, unsigned int fragShader)   -> void;
        auto Compile(GLuint shader)                                         -> void;
        auto Link()                                                         -> void;
        auto checkShaderCompileStatus(const GLuint &shader)                 -> void;
        auto checkProgramLinkStatus(const GLuint &program)                  -> void;
    
    private:
        const GLuint ProgramID;
        std::string name;
        mutable std::unordered_map<std::string, GLuint> UniformLocations;
        //need  shader handle
        //need  shader type
};
