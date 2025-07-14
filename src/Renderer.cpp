#include <core/OpenGL.hpp>
#include <core/Log.hpp>
#include <core/Renderer.hpp>
#include <core/Scene.hpp>
#include <core/GameObject.hpp>
#include <core/Camera.hpp>

Renderer::Renderer() {
}
Renderer::~Renderer(){
}


auto Renderer::render(Scene &scene, Camera &camera) -> void
{
    scene.skyBox()->render(camera);
    //Drwaing        
    for(auto &obj: scene.Entities()){
        auto material = obj.material();
        auto TextureUnit = material->texture()->TextureUnit();
        material->Use();
        material->SetUniform("View", camera.View());
        material->SetUniform("Projection", camera.Perspective());
        material->SetUniform("cameraPos", camera.Position());
        material->SetUniform("skybox", scene.skyBox()->gameObject().material()->texture()->TextureUnit());
        material->SetUniform("albedo", TextureUnit);
        auto sizeIns = static_cast<GLsizei>(obj.InstancePos().size());
        auto mesh = obj.mesh();
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
    mesh.DisableAttribs();
}