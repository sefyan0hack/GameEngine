#pragma once

#include <core/GameObject.hpp>
#include <core/fmts.hpp>

/// @brief  Scene has all the Entities to Render + Sky box
class Scene
{
public:
    friend struct std::formatter<Scene>;
    Scene();
    ~Scene();
    auto operator << (GameObject obj) -> void;

    auto Entities() const -> std::span<const GameObject>;
    auto SetSkyBox(const std::string& BasePathName) -> void;
    auto SetSkyBox(std::shared_ptr<TextureCubeMap> texture) -> void;
    auto SkyBox() const -> const std::unique_ptr<class SkyBox>&;
    auto Clear() -> void;

private:
    std::unique_ptr<class SkyBox> m_SkyBox;
    std::vector<GameObject> m_Entities;

    FOR_TEST
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