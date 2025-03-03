#pragma once
#include <glm/glm.hpp>
#include <format>
#include <vector>

// custom glm::vec2 Format
template<>
struct std::formatter<glm::vec2> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const glm::vec2& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "x": {}, "y": {} }})"
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
    R"({{ "x": {}, "y": {}, "z": {} }})"
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
  "[[ {}, {}, {}, {} ], [ {}, {}, {}, {} ], [ {}, {}, {}, {} ], [ {}, {}, {}, {} ]]"
    ,
    obj[0][0], obj[1][0], obj[2][0], obj[3][0],
    obj[0][1], obj[1][1], obj[2][1], obj[3][1],
    obj[0][2], obj[1][2], obj[2][2], obj[3][2],
    obj[0][3], obj[1][3], obj[2][3], obj[3][3]);
  }
};

// custom maps formater

template<typename Map>
struct MapWrapper {
    const Map& map;
};

template<typename Wrapper>
struct MapFormatter : std::formatter<std::string> {
    auto format(const Wrapper& wrapper, std::format_context& ctx) const {
        std::string result = "{ ";
        bool first = true;
        for (const auto& [key, value] : wrapper.map) {
            if (!first) {
              result += ", ";
            }
            result += std::format(R"("{}": "{}")", key, value);
            first = false;
        }
        result += " }";
        return std::formatter<std::string>::format(result, ctx);
    }
};

template<typename Map>
struct std::formatter<MapWrapper<Map>> : MapFormatter<MapWrapper<Map>> {};


template <typename T>
struct VecWrapper {
    std::vector<T> vec;
};

template <typename T>
struct std::formatter<VecWrapper<T>, char> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const VecWrapper<T>& w, FormatContext& ctx) const {
        auto out = ctx.out();
        *out++ = '{';
        out = std::format_to(out, "\"vec\": [");
        bool first = true;
        for (const auto& item : w.vec) {
            if (!first)
                out = std::format_to(out, ", ");
            first = false;
            out = std::format_to(out, "{}", item);
        }
        out = std::format_to(out, "]");
        *out++ = '}';
        return out;
    }
};