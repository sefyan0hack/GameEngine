#pragma once
#include <memory>
#include "Renderer.hpp"
#include <engine_export.h>
#include <cstdint>

class ENGINE_EXPORT OpenGLRenderer final: public IRenderer
{
public:
    OpenGLRenderer(const class OpenGL& ctx);
    
    auto render(const class Scene& scene) const -> void override;

    auto viewport() const -> std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> override;
    auto set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void override;
    auto normal_mode() -> void override;
    auto wireframe_mode() -> void override;
    auto points_mode() -> void override;
    auto clear_screen(uint32_t buffersmask)  -> void  override;
    auto has_extension(const std::string& ext) -> bool override;

protected:
    auto draw(const class Mesh& mesh) const -> void override;

private:
    const class OpenGL& m_GApi;
    uint32_t m_X, m_Y, m_Width, m_Height;
    std::shared_ptr<class Shader> m_Vert, m_Frag;
    std::shared_ptr<class ShaderProgram> m_Program;
};