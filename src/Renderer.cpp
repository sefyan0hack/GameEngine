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
        obj.Render();
    }
}