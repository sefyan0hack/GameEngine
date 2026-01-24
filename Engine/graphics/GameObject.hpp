#pragma once
#include <memory>
#include <string>
#include <format>

#include <glm/fwd.hpp>
#include "Transform.hpp"
#include <engine_export.h>

class ENGINE_EXPORT GameObject {
public:
    friend struct std::formatter<GameObject>;
    GameObject(glm::vec3 position, std::shared_ptr<class Material> matt, std::shared_ptr<class Mesh> mesh) noexcept;
    GameObject(Transform transform, std::shared_ptr<class Material> matt, std::shared_ptr<class Mesh> mesh) noexcept;
    ~GameObject();

    GameObject(const GameObject&) = default;
    auto operator=(const GameObject&) -> GameObject& = default;

    GameObject(GameObject&& other) noexcept;
    auto operator=(GameObject&& other) noexcept -> GameObject&;

    auto set_position(const glm::vec3 &pos)                   -> void ;
    auto set_scale(const glm::vec3 &Scale)                    -> void ;
    auto rotate(float angle, glm::vec3 axis)                 -> void ;
    auto transform() const                                   -> Transform ;
    auto model() const                                       -> glm::mat4 ;
    auto mesh() const                                        -> std::shared_ptr<class Mesh> ;
    auto material() const                                    -> std::shared_ptr<class Material> ;

private:
    Transform m_Transform;
    std::shared_ptr<class Material> m_Material;
    std::shared_ptr<class Mesh> m_Mesh;
};

#ifdef __cpp_lib_formatters
// custom GameObject Format
template<>
struct std::formatter<GameObject> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const GameObject& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "transform": {} }})",
    obj.m_Transform);
  }
};

#else
#error "__cpp_lib_formatters not defined"
#endif