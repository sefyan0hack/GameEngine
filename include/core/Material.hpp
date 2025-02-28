#pragma once
#include <core/gl.h>
#include <string>
#include <unordered_map>
#include <map>
#include <initializer_list>
#include <core/fmts.hpp>
#include <core/Shader.hpp>
#include <core/Texture.hpp>
#include <core/Mesh.hpp>

#include <memory>
#include <format>

class Material
{
public:
    friend struct std::formatter<Material>;
    Material(const Shader& vertex, const Shader& fragment);
    Material(std::initializer_list<Shader> shaders);

    Material(const Material& other);
    Material(Material&& other);
    ~Material();

    auto id() const noexcept -> GLuint ;
    auto Use() const -> void ;
    auto UnUse() const -> void ;
    auto UniformCount() const                  -> GLint ;
    auto AttribsCount() const                  -> GLint ;
    auto UniformLocation(const char*) const -> GLuint;
    auto AttribLocation(const char*) const -> GLuint;
    auto Uniforms() const noexcept -> const std::map<std::string, GLuint>&;
    auto Attribs() const noexcept -> const std::map<std::string, GLuint>&;
    static auto Current_Program() -> GLuint;
    auto texture() const noexcept -> std::shared_ptr<Texture>;
    auto SetTexture(const std::string &name) -> void;
    auto SetTexture(const std::vector<std::string> faces) -> void;
    auto Shaders() const noexcept -> const std::vector<Shader>&;
    // auto EnableAttribs() const -> void;
    // auto DisableAttribs() const -> void;


    template<class T>
    auto SetUniform(const std::string &name, const T &value) const -> void;

    template<class T>
    auto SetUniform(const std::string &name, const T& value1, const T& value2) const -> void;

    template<class T>
    auto SetUniform(const std::string &name, const T& value1, const T& value2, const T& value3) const -> void;

    template<class T>
    auto SetUniform(const std::string &name, const T& value1, const T& value2, const T& value3, const T& value4) const -> void;

private:
    auto checkProgramLinkStatus() const        -> void;
    auto Link() const -> void;
    auto UniformLocation_Prv(const char* name) const -> GLuint;
    auto AttribLocation_Prv(const char* name) const -> GLuint;
    auto DumpUniforms()                   -> void ;
    auto DumpAttribs()                    -> void ;

private:
    GLuint m_Id;
    std::vector<Shader> m_Shaders;
    std::map<std::string, GLuint> m_Attribs;
    std::map<std::string, GLuint> m_Uniforms;
    std::shared_ptr<Texture> m_Albedo;
    mutable GLuint m_Previd;
};

// custom Material Format
template<>
struct std::formatter<Material> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Material& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "{}: {{ id: {}, attribs: {}, uniforms: {} }}"
    , typeid(obj).name(), obj.m_Id, MapWrapper{obj.m_Attribs}, MapWrapper{obj.m_Uniforms});
  }
};