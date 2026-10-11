#pragma once
#include "Renderer.hpp"
#include "OpenGL.hpp"
#include "Mesh.hpp"

#include <engine_export.h>

#include <emath/mat4.hpp>
#include <emath/vec2.hpp>
#include <emath/vec3.hpp>
#include <emath/vec4.hpp>

#include <memory>
#include <cstdint>
#include <unordered_map>
#include <vector>

class ENGINE_EXPORT OpenGLRenderer final: public IRenderer
{
public:
    OpenGLRenderer(const class OpenGL& ctx, class Text& text);

    auto render(const class Scene& scene) const -> void override;

    auto depthpre_pass(const class Scene& scene) const -> void;
    auto scene_pass(const class Scene& scene) const -> void;
    auto skybox_pass() const -> void;
    auto text_pass() const -> void;
    auto ui_pass() const -> void;

    auto set_viewport(int32_t x, int32_t y, int32_t width, int32_t height) -> void override;
    auto set_mode(DrawMode mode) -> void override;
    auto clear_screen(uint32_t buffersmask) const -> void  override;
    auto stats() const -> RenderStats override;
    auto gpu_time_elapsed() const -> std::unordered_map<std::string, uint64_t>& override;

    auto draw_rectangle(emath::vec2 pos, emath::vec2 size, float radius, emath::vec4 color) const-> void;
    auto draw_circle(emath::vec2 pos, float radius, emath::vec4 color) const -> void;

private:
    auto prepare_text_buffers() -> void;

    auto set_depth(bool v) const -> void;
    auto set_stencil(bool v) const -> void;
    auto set_blend(bool v) const -> void;
    auto set_face_cull(bool v) const -> void;

private:
    const class OpenGL& m_GApi;
    DrawMode m_DrawMode;

    struct {
        std::shared_ptr<class ShaderProgram> Program;
        uint32_t time_elapsed;
        mutable std::vector<emath::mat4> model_data;
    } m_Depth;

    struct {
        std::shared_ptr<class ShaderProgram> Program;
        uint32_t time_elapsed;
        mutable std::vector<emath::mat4> model_data;
        mutable std::vector<int32_t> tex_data;
    } m_Scene;

    struct {
        std::shared_ptr<class ShaderProgram> Program;
        std::shared_ptr<class Texture> Texture;
        uint32_t time_elapsed;
    } m_SkyBox;

    struct {
        class ::Text& Text;
        std::shared_ptr<class ShaderProgram> Program;
        uint32_t VAO, VBO, Atlas;
        uint32_t time_elapsed;
    } m_Text;

    struct {
        std::shared_ptr<class ShaderProgram> Program;
    } m_Ui;

    mutable RenderStats m_Stats;
    mutable uint32_t m_Frame;
    mutable std::unordered_map<std::string, uint64_t> m_Gpu_time_elaped;
};