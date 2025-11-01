#pragma once

#include <memory>
#include "OpenGL.hpp"

class Renderer
{
public:
    Renderer(const class CWindow& window);
    ~Renderer();
    
    auto render(const class Scene& scene, std::shared_ptr<class ShaderProgram> program) const -> void;

    auto graphic_api() const -> const gl::OpenGL&;
    auto set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void;
    auto enable_wireframe() -> void;
    auto disable_wireframe() -> void;
    auto enable_points() -> void;
    auto disable_points() -> void;
    auto clear_screen(GLenum buffersmask)  -> void ;
    auto has_extension(const std::string& ext) -> bool;

private:
    auto draw(const class Mesh& mesh) const -> void;

private:
    const class CWindow& m_Window;
    const gl::OpenGL m_GraphicApi;
};
