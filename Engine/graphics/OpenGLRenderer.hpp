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
    auto skybox_pass() const -> void;
    auto text_pass() const -> void;

    auto set_viewport(int32_t x, int32_t y, int32_t width, int32_t height) -> void override;
    auto set_mode(DrawMode mode) -> void override;
    auto clear_screen(uint32_t buffersmask) const -> void  override;
    auto stats() const -> RenderStats override;

    constexpr static size_t TEXT_BATCH_SIZE = 4096;

private:
    auto prepare_text_buffers() -> void;

    auto set_depth(bool v) const -> void;
    auto set_stencil(bool v) const -> void;
    auto set_blend(bool v) const -> void;
    auto set_face_cull(bool v) const -> void;

private:
    const class OpenGL& m_GApi;
    DrawMode m_DrawMode;
    mutable RenderStats m_Stats;

    struct {
        std::shared_ptr<class ShaderProgram> Program;
    } m_Depth;

    struct {
        std::shared_ptr<class ShaderProgram> Program;
    } m_Scene;

    struct {
        std::shared_ptr<class ShaderProgram> Program;
        std::shared_ptr<class TextureCubeMap> Texture;
    } m_SkyBox;

    struct {
        class ::Text& Text;
        std::shared_ptr<class ShaderProgram> Program;
        uint32_t VAO, VBO, Atlas;
    } m_Text;

    uint32_t m_CameraUBO, m_SunUBO;
};