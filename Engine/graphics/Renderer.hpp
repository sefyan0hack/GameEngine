#pragma once

#include <engine_export.h>
#include <string>
#include <cstdint>

enum class DrawMode {
    Triangles = 0,
    Line,
    Point,
};

class ENGINE_EXPORT IRenderer
{
public:
    virtual ~IRenderer() = default;

    virtual auto render(const class Scene& scene) const -> void = 0;

    virtual auto viewport() const -> std::tuple<int32_t, int32_t, int32_t, int32_t> = 0;
    virtual auto set_viewport(int32_t x, int32_t y, int32_t width, int32_t height) -> void = 0;
    virtual auto set_mode(DrawMode mode) -> void = 0;
    virtual auto clear_screen(uint32_t buffersmask)  -> void  = 0;
    virtual auto has_extension(const std::string& ext) -> bool = 0;

};
