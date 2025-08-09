#pragma once
#include <core/OpenGL.hpp>
#include <core/fmts.hpp>
#include <core/ShaderProgram.hpp>
#include <core/Texture.hpp>
#include <core/Mesh.hpp>

class Material
{
public:
                              //loc    type    size
    using GLSLVar = std::tuple<GLuint, GLenum, GLsizei>;

    friend struct std::formatter<Material>;
    Material();

    Material(const Material& other);
    Material(std::shared_ptr<Texture> diffuse);

    Material(Material&& other) noexcept;
    ~Material();

    auto Bind(std::shared_ptr<ShaderProgram> program) const -> void;

    auto texture(const std::string& name) const noexcept -> std::shared_ptr<Texture>;
    auto SetTexture(const std::string &name, std::shared_ptr<Texture> texture) -> void;
    auto SetDiffuse(std::shared_ptr<Texture> texture) -> void;
    auto RemoveTexture(const std::string &name) -> bool;

private:
    std::map<std::string, std::shared_ptr<Texture>> m_Textuers;

    FOR_TEST
};
