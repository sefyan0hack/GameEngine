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

    auto bind(std::shared_ptr<class ShaderProgram> program) -> void;
    auto diffuse() const noexcept -> std::shared_ptr<class Texture>;
    auto set_diffuse(std::shared_ptr<class Texture> texture) -> void;

    static auto set_skybox(const std::string& BasePathName) -> void;
    static auto set_skybox(std::shared_ptr<class TextureCubeMap> texture) -> void;
    static auto render_sky(const class Camera& cam) -> void;

    static auto skybox() -> class SkyBox&;
private:
    std::shared_ptr<class Texture> m_Diffuse;
};
