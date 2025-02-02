#pragma once
#include <glm/glm.hpp>
#include <format>
#include <unordered_map>

// custom glm::vec2 Format
template<>
struct std::formatter<glm::vec2> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const glm::vec2& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "vec2{{ x: {}, y: {} }}"
    , obj.x, obj.y);
  }
};

// custom glm::vec3 Format
template<>
struct std::formatter<glm::vec3> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const glm::vec3& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "vec3{{ x: {}, y: {}, z: {} }}"
    , obj.x, obj.y, obj.z);
  }
};

// custom glm::mat4 Format
template<>
struct std::formatter<glm::mat4> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const glm::mat4& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "mat4:\n"
    " {{ a1: {:>2}, b1: {:>2}, c1: {:>2}, d1: {:>2} }}\n"
    " {{ a2: {:>2}, b2: {:>2}, c2: {:>2}, d2: {:>2} }}\n"
    " {{ a3: {:>2}, b3: {:>2}, c3: {:>2}, d3: {:>2} }}\n"
    " {{ a4: {:>2}, b4: {:>2}, c4: {:>2}, d4: {:>2} }}\n"
    ,
    obj[0][0], obj[1][0], obj[2][0], obj[3][0],
    obj[0][1], obj[1][1], obj[2][1], obj[3][1],
    obj[0][2], obj[1][2], obj[2][2], obj[3][2],
    obj[0][3], obj[1][3], obj[2][3], obj[3][3]);
  }
};

// custom unordered_map Format

template<typename K, typename V>
struct UnorderedMapWrapper {
    const std::unordered_map<K, V>& map;
};

template<typename K, typename V>
struct std::formatter<UnorderedMapWrapper<K, V>> : std::formatter<std::string> {
    auto format(const UnorderedMapWrapper<K, V>& wrapper, std::format_context& ctx) const {
        std::string result = "{ ";
        bool first = true;
        for (const auto& [key, value] : wrapper.map) {
            if (!first) {
                result += ", ";
            } else {
                first = false;
            }
            result += std::format("[{} : {}]", key, value);
        }
        result += " }";
        return std::formatter<std::string>::format(result, ctx);
    }
};
