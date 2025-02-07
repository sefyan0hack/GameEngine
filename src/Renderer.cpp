#include <core/gl.h>
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
    //Drwaing    
    auto objs = scene.GetGameObjects();
    for(auto &obj: objs){
        obj.Bind();
        auto material = obj.GetMaterial();
        material->Use();
        material->SetUniform("Perspective", camera.GetPerspective());
        material->SetUniform("View", camera.GetView());
        auto sizeIns = static_cast<GLsizei>(obj.GetInstancePos().size());
        auto& mesh = obj.GetMesh();
        draw(mesh, sizeIns);
    }
}

auto Renderer::draw(const Mesh& mesh, GLsizei count) -> void
{
    if(count > 1){
        #ifdef USE_EBO
                glDrawElementsInstanced(GL_TRIANGLES, mesh.vInSize, GL_UNSIGNED_INT, 0, count);
        #else
                glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.vInSize, count);
        #endif
    }else{
        #ifdef USE_EBO
                glDrawElements(GL_TRIANGLES, mesh.vInSize, GL_UNSIGNED_INT, 0);
        #else
                glDrawArrays(GL_TRIANGLES, 0, mesh.vInSize);
        #endif
    }
}