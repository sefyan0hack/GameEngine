#pragma once
#include <version>
#include <variant>
#include <format>
#include <utility>

#include <emath/emath.hpp>

#ifdef __cpp_lib_formatters

// custom emath::vec2 Format
template<class... Ts>
struct std::formatter<std::variant<Ts...>> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const std::variant<Ts...>& obj, auto& context) const {
    return std::visit(
        [&context](const auto& i){ return std::format_to(context.out(), "{}", i); },
        obj
    );
  }
};

// custom emath::vec2 Format
template<>
struct std::formatter<emath::vec2> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const emath::vec2& obj, auto& context) const {
    return std::format_to(context.out(),
    R"({{ "x": {}, "y": {} }})"
    , obj.x, obj.y);
  }
};

// custom emath::vec3 Format
template<>
struct std::formatter<emath::vec3, char> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const emath::vec3& obj, auto& context) const {
    return std::format_to(context.out(),
    R"({{ "x": {}, "y": {}, "z": {} }})"
    , obj.x, obj.y, obj.z);
  }
};

// custom emath::mat4 Format
template<>
struct std::formatter<emath::mat4> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const emath::mat4& obj, auto& context) const {
    return std::format_to(context.out(),
  "[[ {}, {}, {}, {} ], [ {}, {}, {}, {} ], [ {}, {}, {}, {} ], [ {}, {}, {}, {} ]]"
    ,
    obj[0][0], obj[1][0], obj[2][0], obj[3][0],
    obj[0][1], obj[1][1], obj[2][1], obj[3][1],
    obj[0][2], obj[1][2], obj[2][2], obj[3][2],
    obj[0][3], obj[1][3], obj[2][3], obj[3][3]);
  }
};

#endif