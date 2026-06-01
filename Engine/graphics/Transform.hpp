#pragma once
#include <format>

#include <emath/vec3.hpp>
#include <emath/mat4.hpp>

#include <engine_export.h>

class ENGINE_EXPORT Transform
{
public:
    friend struct std::formatter<Transform>;
    Transform();
    Transform(emath::vec3 pos, emath::vec3 rot = {0.0f, 0.0f, 0.0f}, emath::vec3 s = {1.0f, 1.0f, 1.0f});

    auto operator=(const emath::mat4& M) -> Transform;
    operator emath::mat4 () const;

    static auto to_mat4(const Transform& t) -> emath::mat4;
    static auto from_mat4(const emath::mat4& M) -> Transform;

public:
    emath::vec3 position;
    emath::vec3 rotation;
    emath::vec3 scale;
};

#ifdef __cpp_lib_formatters
// custom Transform Format
template<>
struct std::formatter<Transform> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Transform& obj, auto& context) const {
    return std::format_to(context.out(),
    R"({{ "position": {}, "rotation": {}, "scale": {} }})"
    , obj.position, obj.rotation, obj.scale);
  }
};
#endif