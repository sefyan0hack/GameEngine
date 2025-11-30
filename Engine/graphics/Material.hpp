#pragma once
#include <tuple>
#include <memory>
#include <string>
#include <map>

#include "gl.hpp"
#include "ShaderProgram.hpp"
#include <engine_export.h>

class ENGINE_EXPORT Material
{
public:
                              //loc    type    size
    using GLSLVar = std::tuple<GLuint, GLenum, GLsizei>;

    friend struct std::formatter<Material>;
    Material();
    Material(std::shared_ptr<class Texture> diffuse);

    auto bind(std::shared_ptr<class ShaderProgram> program) const -> void;
    auto texture(const std::string& name) const noexcept -> std::shared_ptr<class Texture>;
    auto set_texture(const std::string &name, std::shared_ptr<class Texture> texture) -> void;
    auto set_diffuse(std::shared_ptr<class Texture> texture) -> void;
    auto remove_texture(const std::string &name) -> bool;

private:
    std::map<std::string, std::shared_ptr<class Texture>> m_Textuers;
};
