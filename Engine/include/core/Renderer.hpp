#pragma once

#include <memory>
#include <core/OpenGL.hpp>

class Renderer
{
public:
    Renderer(const class CWindow& window, const class Camera& camera);
    ~Renderer();
    
    auto Render(const class Scene& scene, std::shared_ptr<class ShaderProgram> program) -> void;
    auto RenderSky(const class Scene& scene)      -> void;

    auto opengl() const -> const gl::OpenGL&;
    auto SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void;

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
