#pragma once

#include <core/OpenGL.hpp>

class Scene;
class Camera;
class Mesh;
class ShaderProgram;

class Renderer
{
public:
    Renderer(const Scene& scene);
    ~Renderer();
    
    auto Render(Camera &camera, std::shared_ptr<ShaderProgram> program) -> void;
    auto RenderSky(Camera &camera)      -> void;

    // TODO: add it later
    // auto ChangeScene(Camera &camera, std::shared_ptr<ShaderProgram> program) -> void;
private:
    auto draw(const Mesh& mesh, GLsizei count = 1) -> void;

private:
    const Scene& m_Scene;

    FOR_TEST
};
