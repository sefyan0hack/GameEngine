#pragma once
#include "Renderer.hpp"

#include <engine_export.h>

#include <memory>
#include <cstdint>

class ENGINE_EXPORT OpenGLRenderer final: public IRenderer
{
public:
    OpenGLRenderer(const class OpenGL& ctx, class Text& text);

    auto render(const class Scene& scene) const -> void override;

    auto depthpre_pass(const class Scene& scene) const -> void;
    auto scene_pass(const class Scene& scene) const -> void;
    auto skybox_pass(const class Camera& cam) const -> void;
    auto text_pass() const -> void;

    auto viewport() const -> std::tuple<int32_t, int32_t, int32_t, int32_t> override;
    auto set_viewport(int32_t x, int32_t y, int32_t width, int32_t height) -> void override;
    auto set_mode(DrawMode mode) -> void override;
    auto clear_screen(uint32_t buffersmask) const -> void  override;
    auto extension_supported(const std::string& ext) -> bool override;
    auto render_stats() const -> RenderStats override;

    constexpr static size_t TEXT_BATCH_SIZE = 4096;

private:
    auto prepare_text_buffers() -> void;
private:
    const class OpenGL& m_GApi;
    class Text& m_Text;
    int32_t m_X, m_Y, m_Width, m_Height;
    std::shared_ptr<class ShaderProgram> m_ProgramDepth;
    std::shared_ptr<class ShaderProgram> m_ProgramScene;
    std::shared_ptr<class ShaderProgram> m_ProgramSkyBox;
    std::shared_ptr<class ShaderProgram> m_ProgramText;
    std::shared_ptr<class TextureCubeMap> m_SkyBoxTexture;
    uint32_t VAO, VBO, m_TextAtlas;
    DrawMode m_DrawMode;
    mutable RenderStats m_Stats;
};