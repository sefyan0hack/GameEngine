#pragma once
#include <core/OpenGL.hpp>
#include <string>
class Shader;

struct Shaders_Buffer{
    Shader *vertex, *fragment; // vertex and fragment shader for now
};

class Material
{
public:
    Material(Shader& vertex, Shader& fragment);
    ~Material();

    auto Getid() const -> unsigned int ;
    auto Use() const -> void ;
    auto Link() const -> void;
    auto GetUniformLocation(const char*) const -> unsigned int;
    auto checkProgramLinkStatus() const        -> void;

    template<class T>
    auto SetUniform(const std::string &name, const T &value) const -> void;

    template<class T>
    auto SetUniform(const std::string &name, const T& value1, const T& value2) const -> void;

    template<class T>
    auto SetUniform(const std::string &name, const T& value1, const T& value2, const T& value3) const -> void;

    template<class T>
    auto SetUniform(const std::string &name, const T& value1, const T& value2, const T& value3, const T& value4) const -> void;
private:
    unsigned int id;
    Shaders_Buffer Shaders;
};