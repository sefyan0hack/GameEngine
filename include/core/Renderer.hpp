#pragma once

#include <core/gl.h>

class Scene;
class Camera;
class Mesh;

class Renderer
{
  FOR_TEST(Renderer)
    public:
        Renderer();
        ~Renderer();
        
        auto render(Scene &scene, Camera &camera) -> void;
    private:
        auto draw(const Mesh& mesh, GLsizei count = 1) -> void;
        
};
