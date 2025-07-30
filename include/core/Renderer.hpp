#pragma once

#include <core/OpenGL.hpp>

class Scene;
class Camera;
class Mesh;
class ShaderProgram;

class Renderer
{
    public:
        Renderer();
        ~Renderer();
        
        auto render(Scene &scene, Camera &camera, std::shared_ptr<ShaderProgram> program) -> void;
    private:
        auto draw(const Mesh& mesh, GLsizei count = 1) -> void;
        FOR_TEST  
};
