#include <glad/glad.h>
#include <core/Global_H.hpp>
#include <core/Renderer.hpp>
#include <core/Scene.hpp>
#include <core/GameObject.hpp>
#include <core/Camera.hpp>

Renderer::Renderer(Scene &scene, Camera &camera): scene(scene), cam(camera)  {
}
Renderer::~Renderer(){
}


auto Renderer::render() -> void
{
    //Drwaing    
    auto objs = scene.GetGameObjects();
    for(auto &obj: objs){
        obj.GetMaterial().Use();
        auto sizeIns = obj.GetInstancePos().size();
        auto mesh = obj.GetMesh();
        glBindVertexArray(mesh.VAO);

        if(sizeIns > 1){
            #ifdef USE_EBO
                    glDrawElementsInstanced(GL_TRIANGLES, vInSize, GL_UNSIGNED_INT, 0, sizeIns);
            #else
                    glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.vInSize, sizeIns);
            #endif
                }else{
            #ifdef USE_EBO
                    glDrawElements(GL_TRIANGLES, mesh.vInSize, GL_UNSIGNED_INT, 0);
            #else
                    glDrawArrays(GL_TRIANGLES, 0, mesh.vInSize);
            #endif
        }

        glBindVertexArray(0);
    }
}