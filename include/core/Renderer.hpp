#pragma once
#include <string>

class Scene;
class Camera;
class Mesh;

class Renderer
{    
    public:
        Renderer();
        ~Renderer();
        
        auto render(Scene &scene, Camera &camera) -> void;
    private:
        auto draw(const Mesh& mesh, size_t count = 1) -> void;
        
};
