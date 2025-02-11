#include <core/gl.h>
#include <core/Log.hpp>
#include <core/Renderer.hpp>
#include <core/Scene.hpp>
#include <core/GameObject.hpp>
#include <core/Camera.hpp>
#include <ranges>

Renderer::Renderer() {
}
Renderer::~Renderer(){
}


auto Renderer::render(Scene &scene, Camera &camera) -> void
{
    //Drwaing    
    auto objs = scene.GetGameObjects();

    // render sky box
    auto skybox = objs.front();
    auto skymaterial = skybox.GetMaterial();
    auto skymesh = skybox.GetMesh();

    glDepthFunc(GL_LEQUAL);
    skybox.Bind();
    skymaterial->Use();
    skymaterial->SetUniform("View", glm::mat4(glm::mat3(camera.GetView())));
    skymaterial->SetUniform("Perspective", camera.GetPerspective());
    skymaterial->EnableAttribs();
    draw(*skymesh.get());
    glDepthFunc(GL_LESS);
    
    for(auto &obj: objs | std::views::drop(1)){
        obj.Bind();
        auto material = obj.GetMaterial();
        material->Use();
        material->SetUniform("View", camera.GetView());
        material->SetUniform("Perspective", camera.GetPerspective());
        material->SetUniform("cameraPos", camera.GetPosition());
        auto sizeIns = static_cast<GLsizei>(obj.GetInstancePos().size());
        auto mesh = obj.GetMesh();
        material->EnableAttribs();
        draw(*mesh.get(), sizeIns);
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