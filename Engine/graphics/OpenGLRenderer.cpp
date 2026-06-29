#include "OpenGLRenderer.hpp"

#include <algorithm>

#include "Window.hpp"
#include "Camera.hpp"
#include "OpenGL.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"

#include <core/Log.hpp>

OpenGLRenderer::OpenGLRenderer(const OpenGL& ctx)
    : m_GApi(ctx)
    , m_X(0), m_Y(0)
    , m_Width(ctx.window().dims().first), m_Height(ctx.window().dims().second)
    , m_ProgramScene(std::make_shared<ShaderProgram>("res/Shaders/main.vert", "res/Shaders/main.frag"))
    , m_ProgramSkyBox(std::make_shared<ShaderProgram>("res/Shaders/skybox.vert", "res/Shaders/skybox.frag"))
    , m_SkyBoxTexture(std::make_shared<TextureCubeMap>(TextureCubeMap::base_to_6facesfiles("res/forest.jpg")))
    , m_DrawMode(DrawMode::Triangles)
{}

auto OpenGLRenderer::render(const Scene& scene) const -> void
{
    render_scene(scene);
    render_skybox(scene.main_camera());
}

auto OpenGLRenderer::render_scene(const Scene& scene) const -> void
{
    m_Stats.reset();

    auto camera = scene.main_camera();

    m_ProgramScene->use();
    m_ProgramScene->set_uniform("View", camera.view());
    m_ProgramScene->set_uniform("Projection", camera.projection());
    m_Stats.shaderBinds++;

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

            m_ProgramScene->set_uniform("Model", obj.model());

            // Bind material only when it changes
            if (currentMaterial != material)
            {
                currentMaterial = material;
                currentMaterial->bind(m_ProgramScene);
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

auto OpenGLRenderer::render_skybox(const Camera& cam) const -> void
{
    GLint old_depth_func{};
    gl::GetIntegerv(GL_DEPTH_FUNC, &old_depth_func);
    GLboolean old_depth_mask{};
    gl::GetBooleanv(GL_DEPTH_WRITEMASK, &old_depth_mask);

    gl::DepthFunc(GL_LEQUAL);
    gl::DepthMask(GL_FALSE);

    m_ProgramSkyBox->use();

    m_ProgramSkyBox->set_uniform("u_InvProjection", cam.perspective().inverse());
    m_ProgramSkyBox->set_uniform("u_InvViewRot", emath::mat3(cam.view()).transpose());

    gl::ActiveTexture(GL_TEXTURE0);
    m_SkyBoxTexture->bind();
    m_ProgramSkyBox->set_uniform("uDiffuseMap", 0);

    // gl::BindVertexArray(VAO);
    gl::DrawArrays(GL_TRIANGLES, 0, 3);

    gl::DepthFunc(old_depth_func);
    gl::DepthMask(old_depth_mask);
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

auto OpenGLRenderer::clear_screen(uint32_t buffersmask) const -> void
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