#pragma once
#include "Renderer.hpp"
#include <engine_export.h>

class ENGINE_EXPORT OpenGLRenderer final: public IRenderer
{
public:
    OpenGLRenderer(const class OpenGL& ctx);
    
    auto render(const class Scene& scene, std::shared_ptr<class ShaderProgram> program) const -> void override;

    auto graphic_api() const -> std::string_view override;
    auto viewport() const -> std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> override;
    auto set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void override;
    auto enable_wireframe() -> void override;
    auto disable_wireframe() -> void override;
    auto enable_points() -> void override;
    auto disable_points() -> void override;
    auto clear_screen(uint32_t buffersmask)  -> void  override;
    auto has_extension(const std::string& ext) -> bool override;

protected:
    auto draw(const class Mesh& mesh) const -> void override;

private:
    const class OpenGL& m_GApi;
    uint32_t m_X, m_Y, m_Width, m_Height;
};