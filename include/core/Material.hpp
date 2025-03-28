#pragma once
#include <core/gl.h>
#include <core/fmts.hpp>
#include <core/Shader.hpp>
#include <core/Texture.hpp>
#include <core/Mesh.hpp>


class Material
{
public:
    friend struct std::formatter<Material>;
    Material(const Shader& vertex, const Shader& fragment);
    Material(std::initializer_list<Shader> shaders);

    Material(const Material& other);
    Material(Material&& other) noexcept;
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
    auto Shaders() const noexcept -> const std::vector<Shader>&;
    // auto EnableAttribs() const -> void;
    // auto DisableAttribs() const -> void;


    auto SetUniform(const std::string &name, const GLuint &value) const -> void;
    auto SetUniform(const std::string &name, const GLfloat &value) const -> void;
    auto SetUniform(const std::string &name, const GLint &value) const -> void;
    auto SetUniform(const std::string &name, const glm::vec2 &value) const -> void;
    auto SetUniform(const std::string &name, const glm::vec3 &value) const -> void;
    auto SetUniform(const std::string &name, const glm::mat2 &value) const -> void;
    auto SetUniform(const std::string &name, const glm::mat3 &value) const -> void;
    auto SetUniform(const std::string &name, const glm::mat4 &value) const -> void;

    auto SetUniform(const std::string &name, const GLuint& value1, const GLuint& value2) const -> void;
    auto SetUniform(const std::string &name, const GLfloat& value1, const GLfloat& value2) const -> void;
    auto SetUniform(const std::string &name, const GLint& value1, const GLint& value2) const -> void;

    auto SetUniform(const std::string &name, const GLuint& value1, const GLuint& value2, const GLuint& value3) const -> void;
    auto SetUniform(const std::string &name, const GLfloat& value1, const GLfloat& value2, const GLfloat& value3) const -> void;
    auto SetUniform(const std::string &name, const GLint& value1, const GLint& value2, const GLint& value3) const -> void;

    auto SetUniform(const std::string &name, const GLuint& value1, const GLuint& value2, const GLuint& value3, const GLuint& value4) const -> void;
    auto SetUniform(const std::string &name, const GLfloat& value1, const GLfloat& value2, const GLfloat& value3, const GLfloat& value4) const -> void;
    auto SetUniform(const std::string &name, const GLint& value1, const GLint& value2, const GLint& value3, const GLint& value4) const -> void;

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
    R"({{ "id": {}, "attribs": {}, "uniforms": {} }})"
    , obj.m_Id, MapWrapper{obj.m_Attribs}, MapWrapper{obj.m_Uniforms});
  }
};