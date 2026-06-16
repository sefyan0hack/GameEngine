#pragma once
#include <tuple>
#include <memory>
#include <string>
#include <map>

#include "gl.hpp"
#include "ShaderProgram.hpp"
#include <engine_export.h>

constexpr int32_t Diffuse_SLOT = 0;
constexpr int32_t SkyBox_SLOT = 1;

class ENGINE_EXPORT Material
{
public:
                              //loc    type    size
    using GLSLVar = std::tuple<GLuint, GLenum, GLsizei>;

    Material();
    Material(std::shared_ptr<class Texture> diffuse);

    auto bind(std::shared_ptr<class ShaderProgram> program) -> void;
    auto diffuse() const noexcept -> std::shared_ptr<class Texture>;
    auto set_diffuse(std::shared_ptr<class Texture> texture) -> void;

private:
    std::shared_ptr<class Texture> m_Diffuse;
};
