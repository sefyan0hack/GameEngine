#pragma once
#include <core/gl.h>

class Material
{
public:
                              //loc    type    size
    using GLSLVar = std::tuple<GLuint, GLenum, GLsizei>;

    friend struct std::formatter<Material>;
    Material();
    Material(const Material& other);
    Material(std::shared_ptr<class Texture> diffuse);


    // Material(const Material& other);
    // auto operator=(const Material& other) -> Material&;

    // Material(Material&& other) noexcept;
    // auto operator=(Material&& other) noexcept -> Material&;


    auto Bind(std::shared_ptr<class ShaderProgram> program) const -> void;
    auto texture(const std::string& name) const noexcept -> std::shared_ptr<class Texture>;
    auto SetTexture(const std::string &name, std::shared_ptr<class Texture> texture) -> void;
    auto SetDiffuse(std::shared_ptr<class Texture> texture) -> void;
    auto RemoveTexture(const std::string &name) -> bool;

private:
    std::map<std::string, std::shared_ptr<class Texture>> m_Textuers;

    FOR_TEST
};
