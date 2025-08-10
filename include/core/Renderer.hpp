#pragma once

#include <core/OpenGL.hpp>


class Renderer
{
public:
    Renderer(const class CWindow& window, const class Camera& camera);
    ~Renderer();
    
    auto Render(const class Scene& scene, std::shared_ptr<class ShaderProgram> program) -> void;
    auto RenderSky(const class Scene& scene)      -> void;

    auto opengl() const -> const gl::OpenGL&;

    // TODO: add it later
    // auto ChangeScene(Camera &camera, std::shared_ptr<class ShaderProgram> program) -> void;
private:
    auto draw(const class Mesh& mesh) -> void;

private:
    const class CWindow& m_Window;
    const gl::OpenGL m_OpenGl;
    const class Camera& m_Camera;

    FOR_TEST
};
