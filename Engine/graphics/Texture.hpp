#pragma once

#include "Image.hpp"

#include <memory>
#include <string>
#include <format>

#include <engine_export.h>

class ENGINE_EXPORT Texture
{
public:
  friend struct std::formatter<Texture>;

  enum class Type {
    Texture2D,
    TextureCubeMap,
    Texture3D,
    Texture2DArray
  };

  Texture(const char* name, Type type);

  Texture(const Texture&) = delete;
  auto operator=(const Texture&) -> Texture& = delete;

  Texture(Texture&&) noexcept;
  auto operator=(Texture&&) noexcept-> Texture&;

  ~Texture();

public:
  auto id() const -> uint32_t;
  auto bind() const -> void;
  auto type() const -> Type;
  auto type_name() const -> std::string;

  static auto texture_2d(const char* name) -> std::shared_ptr<Texture>;
  static auto texture_cubemap(const char* name) -> std::shared_ptr<Texture>;

  static auto gl_format(Image::Format fmt) -> uint32_t;
  static auto gl_internal_format(Image::Format fmt) -> uint32_t;

  static auto storage2d(const auto* data, uint32_t type, int32_t width, int32_t height, int32_t intformat, uint32_t format) -> void;

private:
  auto make_texture_Texture2D(const char* name = nullptr) -> void;
  auto make_texture_TextureCubeMap(const char* name = nullptr) -> void;
  auto make_texture_Texture3D(const char* name = nullptr) -> void;
  auto make_texture_Texture2DArray(const char* name = nullptr) -> void;
private:
  uint32_t m_Id;
  Type m_Type;
};

#ifdef __cpp_lib_formatters
// custom Texture Format
template<>
struct std::formatter<Texture> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Texture& obj, auto& context) const {
    return std::format_to(context.out(),
    R"({{ "id": {}, "type": "{}" }})"
    , obj.m_Id, obj.type_name());
  }
};
#endif