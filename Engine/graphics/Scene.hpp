#pragma once

#include <memory>
#include <span>
#include <string>
#include <vector>
#include <format>

#include "GameObject.hpp"
#include "Camera.hpp"
#include <core/fmts.hpp>
#include <engine_export.h>

/// @brief  Scene has all the Entities to Render
class ENGINE_EXPORT Scene
{
public:
    friend struct std::formatter<Scene>;
    Scene();    

    auto entities() const -> std::span<const GameObject>;
    auto clear() -> void;
    auto main_camera() -> Camera&;
    auto main_camera() const -> Camera&;

    auto operator<<(GameObject entity) -> Scene&;

auto operator<<(Camera entity) -> Scene&;

private:
    std::vector<Camera> m_Cameras;
    class Camera& m_MainCamera;
    std::vector<GameObject> m_Entities;

};

#ifdef __cpp_lib_formatters
// custom Mesh Format
template<>
struct std::formatter<Scene> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Scene& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "Entities": {} }})"
    , VecWrapper{obj.m_Entities});
  }
};

#else
#error "__cpp_lib_formatters not defined"
#endif