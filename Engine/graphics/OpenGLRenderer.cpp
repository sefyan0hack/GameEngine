#include "OpenGLRenderer.hpp"

#include <algorithm>
#include <span>
#include <utility>

#include "Window.hpp"
#include "Camera.hpp"
#include "GameObject.hpp"
#include "SkyBox.hpp"
#include "OpenGL.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Texture.hpp"

OpenGLRenderer::OpenGLRenderer(const OpenGL& ctx)
    : m_GApi(ctx)
    , m_X(0), m_Y(0)
    , m_Width(ctx.window().dims().first), m_Height(ctx.window().dims().second)
    , m_Vert(std::make_shared<Shader>(res::get("res/Shaders/main.vert"), GL_VERTEX_SHADER))
    , m_Frag(std::make_shared<Shader>(res::get("res/Shaders/main.frag"), GL_FRAGMENT_SHADER))
    , m_Program(std::make_shared<ShaderProgram>(m_Vert, m_Frag))
    , m_DrawMode(DrawMode::Triangles)
{}

auto OpenGLRenderer::render(const Scene& scene) const -> void
{
    auto camera = scene.main_camera();
    Material::render_sky(camera);

    m_Program->use();
    m_Program->set_uniform("View", camera.view());
    m_Program->set_uniform("Projection", camera.projection());
    m_Program->set_uniform("Eye", camera.position());

    //Drwaing
    std::for_each(
        scene.entities().begin(), scene.entities().end(),
        [&](const auto& obj){
            m_Program->set_uniform("Model", obj.model());

            obj.material()->bind(m_Program);

            draw(*obj.mesh());
        }
    );
}

auto OpenGLRenderer::draw(const Mesh& mesh) const -> void
{
    gl::BindVertexArray(mesh.VAO);

    switch(m_DrawMode)
    {
        case DrawMode::Triangles:
            gl::DrawArrays(GL_TRIANGLES, 0, mesh.vertex_size());
            break;
        case DrawMode::Line:
            gl::DrawArrays(GL_LINES, 0, (mesh.vertex_size() / 3) * 6);
        break;
        case DrawMode::Point:
            gl::DrawArrays(GL_POINTS, 0, mesh.vertex_size());
            break;
        default: std::unreachable();
    }
}

auto OpenGLRenderer::set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void
{
    std::tie(m_X, m_Y, m_Width, m_Height) = {x, y, width, height};
    gl::Viewport(m_X, m_Y, m_Width, m_Height);
}

auto OpenGLRenderer::viewport() const -> std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>
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

auto OpenGLRenderer::has_extension(const std::string &ext) -> bool
{
    return m_GApi.has_extension(ext);
}