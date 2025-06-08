#pragma once
#include <glm/glm.hpp>


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
      using namespace std::ranges;
      auto joined_range = wrapper.map
      | views::transform([](auto elem) { auto& [key, value] = elem; return std::format(R"("{}": "{}", )", key, value); })
      | std::views::join
      | std::ranges::to<std::string>();
      if(!joined_range.empty()) {joined_range.pop_back(); joined_range.pop_back();}
      
      std::string final_str  = "{" + joined_range + "}";
      return std::formatter<std::string>::format(final_str, ctx);
    }
};

template<typename Map>
struct std::formatter<MapWrapper<Map>> : MapFormatter<MapWrapper<Map>> {};


template <typename Vec>
struct VecWrapper {
    std::vector<Vec> vec;
};

template<typename Wrapper>
struct VecFormatter : std::formatter<std::string> {
    auto format(const Wrapper& wrapper, std::format_context& ctx) const {
      using namespace std::ranges;
      auto joined_range = wrapper.vec
        | views::transform([](auto elem) { return std::format("{}, ", elem); })
        | std::views::join
        | std::ranges::to<std::string>();
      
      if(!joined_range.empty()) {joined_range.pop_back(); joined_range.pop_back();}
      std::string final_str  = "[" + joined_range + "]";
      return std::formatter<std::string>::format(final_str, ctx);
    }
};

template<typename Vec>
struct std::formatter<VecWrapper<Vec>> : VecFormatter<VecWrapper<Vec>> {};