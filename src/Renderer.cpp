#include <core/OpenGL.hpp>
#include <core/Log.hpp>
#include <core/Renderer.hpp>
#include <core/Scene.hpp>
#include <core/GameObject.hpp>
#include <core/Camera.hpp>
#include <core/ShaderProgram.hpp>
#include <core/ResourceManager.hpp>


Renderer::Renderer(const Scene& scene)
    :m_Scene(scene)
{}
Renderer::~Renderer(){
}

auto Renderer::Render(Camera &camera, const std::shared_ptr<ShaderProgram> program) -> void
{
    RenderSky(camera);

    program->Use();
    program->SetUniform("View", camera.View());
    program->SetUniform("Projection", camera.Projection());
    program->SetUniform("Eye", camera.Position());

    //Drwaing

    for(auto &obj: m_Scene.Entities()){
        program->SetUniform("Model", obj.Model());
        obj.material()->Bind(program);
        auto mesh = obj.mesh();
        draw(*mesh);
    }
}

auto Renderer::RenderSky(Camera &camera) -> void
{
    auto& skyBox = m_Scene.SkyBox();

    gl::DepthFunc(GL_LEQUAL);

    skyBox->Program()->Use();
    skyBox->Program()->SetUniform("View", glm::mat4(glm::mat3(camera.View())));
    skyBox->Program()->SetUniform("Projection", camera.Perspective());
    skyBox->Program()->SetUniform("uDiffuseMap", skyBox->texture()->TextureUnit());

    gl::DrawArrays(GL_TRIANGLES, 0, skyBox->mesh().VextexSize());
    gl::DepthFunc(GL_LESS);
}


auto Renderer::draw(const Mesh& mesh) -> void
{
    // mesh.Bind();
    gl::DrawArrays(GL_TRIANGLES, 0, mesh.VextexSize());
}