#pragma once

#include "GameObject.hpp"
#include "Camera.hpp"

#include <engine_export.h>

#include <set>
#include <span>
#include <vector>
#include <format>

struct GameObjectCompare
{
  bool operator()(const GameObject& a, const GameObject& b) const noexcept
  {
    if (a.mesh() != b.mesh())
      return a.mesh() < b.mesh();
    return a.material() < b.material();
  }
};
using EntitiesVector = std::multiset<GameObject, GameObjectCompare>;

/// @brief  Scene has all the Entities to Render
class ENGINE_EXPORT Scene
{
public:
    friend struct std::formatter<Scene>;
    Scene();    

    auto entities() -> EntitiesVector&;
    auto entities() const -> const EntitiesVector&;

    auto clear() -> void;
    auto main_camera() -> Camera&;
    auto main_camera() const -> Camera&;

    auto operator<<(GameObject entity) -> Scene&;

auto operator<<(Camera entity) -> Scene&;

private:
    std::vector<Camera> m_Cameras;
    class Camera& m_MainCamera;
    EntitiesVector m_Entities;
};

#ifdef __cpp_lib_formatters
// custom Mesh Format
template<>
struct std::formatter<Scene> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Scene& obj, auto& context) const {
    return std::format_to(context.out(),
    R"({{ "Entities": {} }})"
    , obj.m_Entities);
  }
};
#endif