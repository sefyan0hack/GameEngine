#pragma once

#include <memory>
#include <span>
#include <string>
#include <vector>
#include <format>

#include "GameObject.hpp"
#include "fmts.hpp"

/// @brief  Scene has all the Entities to Render + Sky box
class Scene
{
public:
    friend struct std::formatter<Scene>;
    Scene(const class Camera& camera);
    ~Scene();
    auto Add(GameObject&& entity) -> void;
    auto Entities() const -> std::span<const GameObject>;
    auto SetSkyBox(const std::string& BasePathName) -> void;
    auto SetSkyBox(std::shared_ptr<class TextureCubeMap> texture) -> void;
    auto RenderSky() const -> void;
    auto Clear() -> void;

private:
    const class Camera& m_Camera;
    std::unique_ptr<class SkyBox> m_SkyBox;
    std::vector<GameObject> m_Entities;

    FOR_TEST
};

auto operator<<(Scene& scene, GameObject&& entity) -> Scene&;

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