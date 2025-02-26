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

    auto Getid() const noexcept -> GLuint ;
    auto Use() const -> void ;
    auto UnUse() const -> void ;
    auto UniformCount() const                  -> GLint ;
    auto AttribsCount() const                  -> GLint ;
    auto GetUniformLocation(const char*) const -> GLuint;
    auto GetAttribLocation(const char*) const -> GLuint;
    auto GetUniforms() const noexcept -> const std::map<std::string, GLuint>&;
    auto GetAttribs() const noexcept -> const std::map<std::string, GLuint>&;
    static auto Current_Program() -> GLuint;
    auto GetTexture() const noexcept -> std::shared_ptr<Texture>;
    auto texture(const std::string &name) -> void;
    auto texture(const std::vector<std::string> faces) -> void;
    // auto GetShaders() const -> std::vector<GLuint>;
    auto GetShaders() const noexcept -> const std::vector<Shader>&;
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
    auto GetUniformLocation_Prv(const char* name) const -> GLuint;
    auto GetAttribLocation_Prv(const char* name) const -> GLuint;
    auto DumpUniforms()                   -> void ;
    auto DumpAttribs()                    -> void ;

private:
    GLuint id;
    std::vector<Shader> Shaders;
    std::map<std::string, GLuint> Attribs;
    std::map<std::string, GLuint> Uniforms;
    std::shared_ptr<Texture> albedo;
    mutable GLuint previd;
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
    , typeid(obj).name(), obj.id, MapWrapper{obj.Attribs}, MapWrapper{obj.Uniforms});
  }
};