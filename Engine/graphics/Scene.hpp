#pragma once

#include <memory>
#include <span>
#include <string>
#include <vector>
#include <format>

#include "GameObject.hpp"
#include "Camera.hpp"
#include <core/fmts.hpp>

/// @brief  Scene has all the Entities to Render + Sky box
class Scene
{
public:
    friend struct std::formatter<Scene>;
    Scene();
    ~Scene();
    auto entities() const -> std::span<const GameObject>;
    auto set_skybox(const std::string& BasePathName) -> void;
    auto set_skybox(std::shared_ptr<class TextureCubeMap> texture) -> void;
    auto render_sky() const -> void;
    auto clear() -> void;
    auto main_camera() -> Camera&;
    auto main_camera() const -> Camera&;

    auto operator<<(GameObject entity) -> Scene&;

auto operator<<(Camera entity) -> Scene&;

private:
    std::vector<Camera> m_Cameras;
    class Camera& m_MainCamera;
    std::unique_ptr<class SkyBox> m_SkyBox;
    std::vector<GameObject> m_Entities;

};

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