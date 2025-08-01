#include <core/OpenGL.hpp>
#include <core/Log.hpp>
#include <core/Renderer.hpp>
#include <core/Scene.hpp>
#include <core/GameObject.hpp>
#include <core/Camera.hpp>
#include <core/ShaderProgram.hpp>

Renderer::Renderer(const Scene& scene)
    :m_Scene(scene)
{}
Renderer::~Renderer(){
}


auto Renderer::Render(Camera &camera, std::shared_ptr<ShaderProgram> program) -> void
{
    m_Scene.SkyBox()->Render(camera);

    program->Use();
    program->SetUniform("View", camera.View());
    program->SetUniform("Projection", camera.Perspective());
    program->SetUniform("Eye", camera.Position());

    // m_Scene.Entities().back().material()->Bind(program);

    //Drwaing
    for(auto &obj: m_Scene.Entities()){
        program->SetUniform("Model", obj.Model());

        auto sizeIns = static_cast<GLsizei>(obj.InstancePos().size());
        auto mesh = obj.mesh();
        obj.material()->Bind(program);
        draw(*mesh.get(), sizeIns);
    }
}

auto Renderer::draw(const Mesh& mesh, GLsizei count) -> void
{
    mesh.Bind();
    if(count > 1){
        #ifdef USE_EBO
        gl::DrawElementsInstanced(GL_TRIANGLES, mesh.VextexSize(), GL_UNSIGNED_INT, 0, count);
        #else
        gl::DrawArraysInstanced(GL_TRIANGLES, 0, mesh.VextexSize(), count);
        #endif
    }else{
        #ifdef USE_EBO
        gl::DrawElements(GL_TRIANGLES, mesh.VextexSize(), GL_UNSIGNED_INT, 0);
        #else
        gl::DrawArrays(GL_TRIANGLES, 0, mesh.VextexSize());
        #endif
    }
}