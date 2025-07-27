#pragma once
#include <glm/glm.hpp>
#include <core/fmts.hpp>

class Transform
{
public:
    friend struct std::formatter<Transform>;
    Transform();
    Transform(glm::vec3 position, glm::vec3 rotaion = {0.0f, 0.0f, 0.0f}, glm::vec3 scale = {1.0f, 1.0f, 1.0f});
    ~Transform() = default;

    auto operator=(const glm::mat4& M) -> Transform;
    operator glm::mat4 () const;

    static auto To_mat4(const Transform& t) -> glm::mat4;
    static auto From_mat4(const glm::mat4& M) -> Transform;

public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    FOR_TEST
};

// custom Transform Format
template<>
struct std::formatter<Transform> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Transform& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "position": {}, "rotation": {}, "scale": {} }})"
    , obj.position, obj.rotation, obj.scale);
  }
};