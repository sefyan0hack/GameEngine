#pragma once

#include <core/GameObject.hpp>
#include <core/fmts.hpp>

class Scene
{
public:
    friend struct std::formatter<Scene>;
    Scene() = default;
    Scene(std::unique_ptr<SkyBox> skybox);
    ~Scene();
    auto operator << (GameObject obj) -> void;

    auto add(GameObject gobj)   -> void;
    auto Entitys()       -> std::vector<GameObject>&;
    auto setSkyBox(std::unique_ptr<SkyBox> skybox) -> void;
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
    R"({{"SkyBox": {}, "Entities": {}}})"
    , static_cast<GameObject>(*obj.m_SkyBox), VecWrapper{obj.m_Entities});
  }
};