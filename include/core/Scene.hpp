#pragma once

#include <core/GameObject.hpp>
#include <core/fmts.hpp>

/// @brief  Scene has all the Entities to render + Sky box
class Scene
{
public:
    friend struct std::formatter<Scene>;
    Scene();
    ~Scene();
    auto operator << (GameObject obj) -> void;

    auto Entities() -> std::vector<GameObject>&;
    auto setSkyBox(const std::string& BasePathName) -> void;
    auto skyBox() -> std::unique_ptr<SkyBox>&;
    auto to_json() const -> std::string;
private:
    std::unique_ptr<SkyBox> m_SkyBox;
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