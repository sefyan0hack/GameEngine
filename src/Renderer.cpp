#include <core/OpenGL.hpp>
#include <core/Log.hpp>
#include <core/Renderer.hpp>
#include <core/Scene.hpp>
#include <core/GameObject.hpp>
#include <core/Camera.hpp>
#include <core/ShaderProgram.hpp>
#include <core/ResourceManager.hpp>
#include <core/Window.hpp>


Renderer::Renderer(const CWindow& window, const Camera& camera)
    : m_Window(window)
    , m_OpenGl(m_Window.Handle(), m_Window.Surface())
    , m_Camera(camera)

{}
Renderer::~Renderer(){
}

auto Renderer::Render(const Scene& scene, const std::shared_ptr<ShaderProgram> program) -> void
{
    RenderSky(scene);

    program->Use();
    program->SetUniform("View", m_Camera.View());
    program->SetUniform("Projection", m_Camera.Projection());
    program->SetUniform("Eye", m_Camera.Position());

    //Drwaing

    for(auto &obj: scene.Entities()){
        program->SetUniform("Model", obj.Model());
        obj.material()->Bind(program);
        auto mesh = obj.mesh();
        draw(*mesh);
    }
}

auto Renderer::RenderSky(const Scene& scene) -> void
{
    auto& skyBox = scene.SkyBox();

    gl::DepthFunc(GL_LEQUAL);

    skyBox->Program()->Use();
    skyBox->Program()->SetUniform("View", glm::mat4(glm::mat3(m_Camera.View())));
    skyBox->Program()->SetUniform("Projection", m_Camera.Perspective());
    skyBox->Program()->SetUniform("uDiffuseMap", skyBox->texture()->TextureUnit());

    gl::BindVertexArray(skyBox->mesh().VAO);
    gl::DrawArrays(GL_TRIANGLES, 0, skyBox->mesh().VextexSize());
    gl::DepthFunc(GL_LESS);
}


auto Renderer::draw(const Mesh& mesh) -> void
{
    gl::BindVertexArray(mesh.VAO);
    gl::DrawArrays(GL_TRIANGLES, 0, mesh.VextexSize());
}

auto Renderer::opengl() const -> const gl::OpenGL&
{
	return m_OpenGl;
}

auto Renderer::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void
{
    gl::Viewport(x, y, width, height);
}