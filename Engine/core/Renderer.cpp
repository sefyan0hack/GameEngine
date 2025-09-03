#include "OpenGL.hpp"
#include "Log.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "GameObject.hpp"
#include "SkyBox.hpp"
#include "Camera.hpp"
#include "ShaderProgram.hpp"
#include "ResourceManager.hpp"
#include "Window.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Texture.hpp"


Renderer::Renderer(const CWindow& window, const Camera& camera)
    : m_Window(window)
    , m_OpenGl(m_Window.Handle(), m_Window.Surface())
    , m_Camera(camera)

{}
Renderer::~Renderer(){
}

auto Renderer::Render(const Scene& scene, const std::shared_ptr<ShaderProgram> program) -> void
{
    scene.RenderSky();

    program->Use();
    program->SetUniform("View", m_Camera.View());
    program->SetUniform("Projection", m_Camera.Projection());
    program->SetUniform("Eye", m_Camera.Position());

    static std::shared_ptr<Material> currMatt = nullptr;
    //Drwaing
    for(auto &obj: scene.Entities()){
        program->SetUniform("Model", obj.Model());

        if(currMatt != obj.material()){
            currMatt = obj.material();
            obj.material()->Bind(program);
        }

        auto mesh = obj.mesh();
        draw(*mesh);
    }
}


auto Renderer::draw(const Mesh& mesh) -> void
{
    static GLuint currVAO = mesh.VAO;
    if(currVAO != mesh.VAO){
        currVAO = mesh.VAO;
        gl::BindVertexArray(currVAO);
    }
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