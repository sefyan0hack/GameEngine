#include "OpenGLRenderer.hpp"

#include <algorithm>
#include <span>
#include <utility>

#include "Window.hpp"
#include "Camera.hpp"
#include "OpenGL.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"

#include <core/Log.hpp>


OpenGLRenderer::OpenGLRenderer(const OpenGL& ctx)
    : m_GApi(ctx)
    , m_X(0), m_Y(0)
    , m_Width(ctx.window().dims().first), m_Height(ctx.window().dims().second)
    , m_Vert(std::make_shared<Shader>("res/Shaders/main.vert", GL_VERTEX_SHADER))
    , m_Frag(std::make_shared<Shader>("res/Shaders/main.frag", GL_FRAGMENT_SHADER))
    , m_Program(std::make_shared<ShaderProgram>(m_Vert, m_Frag))
    , m_DrawMode(DrawMode::Triangles)
{}

auto OpenGLRenderer::render(const Scene& scene) const -> void
{
    m_Stats.reset();

    auto camera = scene.main_camera();

    m_Program->use();
    m_Stats.shaderBinds++;
    m_Program->set_uniform("View", camera.view());
    m_Program->set_uniform("Projection", camera.projection());
    m_Program->set_uniform("Eye", camera.position());

    //Drwaing
    Mesh* currentMesh = nullptr;
    Material* currentMaterial = nullptr;

    std::for_each(
        scene.entities().begin(), scene.entities().end(),
        [&](const auto& obj){

            auto mesh = obj.mesh().get();
            auto material = obj.material().get();

            auto v_count = mesh->vertex_size();
            m_Stats.vertex_cout += v_count;

            m_Program->set_uniform("Model", obj.model());

            // Bind material only when it changes
            if (currentMaterial != material)
            {
                currentMaterial = material;
                currentMaterial->bind(m_Program);
                m_Stats.materialBinds++;
            }

            // Bind mesh only when it changes
            if (currentMesh != mesh)
            {
                currentMesh = mesh;
                gl::BindVertexArray(currentMesh->VAO);
                m_Stats.vaoBinds++;
            }


            switch(m_DrawMode)
            {
                case DrawMode::Triangles:
                    gl::DrawArrays(GL_TRIANGLES, 0, v_count);
                    m_Stats.drawCalls++;
                    break;
                case DrawMode::Line:
                    gl::DrawArrays(GL_LINES, 0, (v_count / 3) * 6);
                    m_Stats.drawCalls++;
                    break;
                case DrawMode::Point:
                    gl::DrawArrays(GL_POINTS, 0, v_count);
                    m_Stats.drawCalls++;
                    break;
            }
        }
    );
}

auto OpenGLRenderer::set_viewport(int32_t x, int32_t y, int32_t width, int32_t height) -> void
{
    std::tie(m_X, m_Y, m_Width, m_Height) = {x, y, width, height};
    gl::Viewport(m_X, m_Y, m_Width, m_Height);
}

auto OpenGLRenderer::viewport() const -> std::tuple<int32_t, int32_t, int32_t, int32_t>
{
    return {m_X, m_Y, m_Width, m_Height};
}

auto OpenGLRenderer::set_mode(DrawMode mode) -> void
{
    m_DrawMode = mode;
}

auto OpenGLRenderer::clear_screen(uint32_t buffersmask)  -> void
{
    gl::Clear((GLenum)buffersmask);
}

auto OpenGLRenderer::extension_supported(const std::string &ext) -> bool
{
    return m_GApi.extension_supported(ext);
}

auto OpenGLRenderer::render_stats() const -> RenderStats
{
    return m_Stats;
}