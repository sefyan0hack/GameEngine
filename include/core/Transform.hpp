#pragma once
#include <glm/glm.hpp>
#include <core/fmts.hpp>
#include <format>

class Transform
{
public:
    friend struct std::formatter<Transform>;
    Transform( glm::vec3 position, glm::vec3 rotaion = {0.0f, 0.0f, 0.0f}, glm::vec3 scale = {1.0f, 1.0f, 1.0f});
    ~Transform() = default;

public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

// custom Transform Format
template<>
struct std::formatter<Transform> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Transform& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "{}: {{ position: {}, rotation: {}, scale: {} }}"
    , typeid(obj).name(), obj.position, obj.rotation, obj.scale);
  }
};