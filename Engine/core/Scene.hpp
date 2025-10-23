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
    Scene(class Camera& camera);
    ~Scene();
    auto add(GameObject&& entity) -> void;
    auto entities() const -> std::span<const GameObject>;
    auto set_skybox(const std::string& BasePathName) -> void;
    auto set_skybox(std::shared_ptr<class TextureCubeMap> texture) -> void;
    auto render_sky() const -> void;
    auto clear() -> void;
    auto main_camera() -> Camera&;

private:
    class Camera& m_Camera;
    std::unique_ptr<class SkyBox> m_SkyBox;
    std::vector<GameObject> m_Entities;

    
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