#pragma once

#include <engine_export.h>
#include <cstdint>
#include <cstddef>

enum class DrawMode {
    Triangles = 0,
    Line,
    Point,
};

struct RenderStats
{
    size_t pipeline_switch = 0;
    size_t texture_switch = 0;
    size_t mesh_switch = 0;
    size_t draw_call = 0;
    size_t vertices = 0;
    size_t indices = 0;

    void reset()
    {
        pipeline_switch = 0;
        texture_switch = 0;
        mesh_switch = 0;
        draw_call = 0;
        vertices = 0;
        indices = 0;
    }
};

class ENGINE_EXPORT IRenderer
{
public:
    virtual ~IRenderer() = default;

    virtual auto render(const class Scene& scene) const -> void = 0;

    virtual auto set_viewport(int32_t x, int32_t y, int32_t width, int32_t height) -> void = 0;
    virtual auto set_mode(DrawMode mode) -> void = 0;
    virtual auto clear_screen(uint32_t buffersmask) const -> void  = 0;
    virtual auto stats() const -> RenderStats = 0;
};
