#pragma once
#include <version>
#include <string>
#include <string_view>
#include <concepts>
#include <tuple>
#include <variant>
#include <format>
#include <utility>
#include "Platform.hpp"
#include <glm/glm.hpp>

#ifndef __cpp_lib_formatters

template<typename T>
requires std::formattable<T, char>
constexpr decltype(auto) make_safe_arg(T&& v) {
    return std::forward<T>(v);
}

template<typename T>
requires (!std::formattable<T, char>)
std::string make_safe_arg(T&&) {
    return std::format("[Unformattable: {}]", ::type_name<std::decay_t<T>>());
}

template <typename... Args>
std::string format(std::format_string<Args...> fmt, Args&&... args) {
    [[maybe_unused]] auto safe_args = std::tuple{ make_safe_arg(std::forward<Args>(args))... };

    return std::apply(
        [&](auto&... unpacked) {
            return std::vformat(
                fmt.get(),
                std::make_format_args(unpacked...)
            );
        },
        safe_args
    );
}

#else

// custom glm::vec2 Format
template<class... Ts>
struct std::formatter<std::variant<Ts...>> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const std::variant<Ts...>& obj, std::format_context& context) const {
    return std::visit(
        [&context](const auto& i){ return std::format_to(context.out(), "{}", i); },
        obj
    );
  }
};

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
struct std::formatter<glm::vec3, char> {
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

template<typename Map>
struct MapWrapper {
    const Map& map;
};

template <typename Vec>
struct VecWrapper {
    const Vec& vec;
};

template <typename Que>
struct QueWrapper {
    const Que& que;
};

// Formatter for MapWrapper
template<typename Map>
struct std::formatter<MapWrapper<Map>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const MapWrapper<Map>& wrapper, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "{{ ");
        bool first = true;
        for (const auto& [key, value] : wrapper.map) {
            if (first) first = false;
            else out = std::format_to(out, ", ");
            out = std::format_to(out, R"("{}": "{}")", key, value);
        }
        out = std::format_to(out, " }}");
        return out;
    }
};

// Formatter for VecWrapper
template<typename Vec>
struct std::formatter<VecWrapper<Vec>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const VecWrapper<Vec>& wrapper, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "[ ");
        bool first = true;
        for (const auto& elem : wrapper.vec) {
            if (first) first = false;
            else out = std::format_to(out, ", ");
            out = std::format_to(out, "{}", elem);
        }
        out = std::format_to(out, " ]");
        return out;
    }
};

// Formatter for QueWrapper
template<typename Que>
struct std::formatter<QueWrapper<Que>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const QueWrapper<Que>& wrapper, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "[ ");
        Que q = wrapper.que;
        bool first = true;
        while (!q.empty()) {
          if (first) first = false;
          else out = std::format_to(out, ", ");
          out = std::format_to(out, "{}", q.front());
          q.pop();
        }
        out = std::format_to(out, " ]");
        return out;
    }
};

template<>
struct std::formatter<TypeInfo> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const TypeInfo& obj, std::format_context& ctx) const {
        return std::format_to(ctx.out(), 
            R"({{ Type: "{}", Parent: "{}", Kind: "{}", Hash: {}, Size: {}, Align: {}, Empty: {} }})",
            obj.name, obj.parent, obj.kind, obj.hash, obj.size, obj.alignment, obj.empty
        );
    }
};

template <typename... Args>
std::string format(std::format_string<Args...> fmt, Args&&... args) { return std::format(fmt, std::forward<Args>(args)...); }

#endif