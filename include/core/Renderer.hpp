#pragma once
#include <string>

class Scene;
class Camera;

class Renderer
{    
    public:
        Renderer(Scene &scene, Camera &camera);
        ~Renderer();
        
        auto render() -> void;
    
    private:
    Scene &scene;
    Camera &cam;
};
