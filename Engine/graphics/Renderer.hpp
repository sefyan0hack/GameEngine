#pragma once

#include <engine_export.h>


class ENGINE_EXPORT IRenderer
{
public:
    virtual ~IRenderer() = default;

    virtual auto render(const class Scene& scene) const -> void = 0;

    virtual auto graphic_api() const -> std::string_view = 0;
    virtual auto viewport() const -> std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> = 0;
    virtual auto set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void = 0;
    virtual auto enable_wireframe() -> void = 0;
    virtual auto disable_wireframe() -> void = 0;
    virtual auto enable_points() -> void = 0;
    virtual auto disable_points() -> void = 0;
    virtual auto clear_screen(uint32_t buffersmask)  -> void  = 0;
    virtual auto has_extension(const std::string& ext) -> bool = 0;
    

protected:
    virtual auto draw(const class Mesh& mesh) const -> void = 0;
};
