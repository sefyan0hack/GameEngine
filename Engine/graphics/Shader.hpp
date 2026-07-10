#pragma once

#include <engine_export.h>

#include <format>
#include <string>
#include <memory>
#include <span>

class ENGINE_EXPORT Shader
{
public:
  enum class Type {
    Vertex,
    Fragment
  };

  friend struct std::formatter<Shader>;

  Shader(const char* shader, Type type);
  Shader(const std::string& shader, Type type);

  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;

  Shader(Shader&& other) noexcept;
  Shader& operator=(Shader&& other) noexcept;

  ~Shader();

public:
  auto id() const                -> uint32_t ;
  auto type() const              -> Type ;
  auto type_name() const         -> const char* ;

  static auto new_vertex(const std::string& vert) -> std::shared_ptr<Shader>;
  static auto new_fragment(const std::string& frag) -> std::shared_ptr<Shader>;

private:
  auto set_sources(const std::span<const char* const> srcs) const -> void;
  auto compile()                           -> void;
  auto check_compile_status() -> std::string;


private:
  uint32_t m_Id;
  Type m_Type;
};

#ifdef __cpp_lib_formatters
// custom Shader Format
template<>
struct std::formatter<Shader> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Shader& obj, auto& context) const {
    return std::format_to(context.out(),
    R"({{ "id": {}, "type": "{}" }})"
    , obj.m_Id, obj.type_name());
  }
};
#endif