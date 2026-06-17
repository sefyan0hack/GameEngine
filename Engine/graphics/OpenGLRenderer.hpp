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

    auto viewport() const -> std::tuple<int32_t, int32_t, int32_t, int32_t> override;
    auto set_viewport(int32_t x, int32_t y, int32_t width, int32_t height) -> void override;
    auto set_mode(DrawMode mode) -> void override;
    auto clear_screen(uint32_t buffersmask)  -> void  override;
    auto has_extension(const std::string& ext) -> bool override;

private:
    const class OpenGL& m_GApi;
    int32_t m_X, m_Y, m_Width, m_Height;
    std::shared_ptr<class Shader> m_Vert, m_Frag;
    std::shared_ptr<class ShaderProgram> m_Program;
    DrawMode m_DrawMode;
};