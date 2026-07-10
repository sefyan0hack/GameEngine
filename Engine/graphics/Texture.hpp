#pragma once

#include "Image.hpp"

#include <string>
#include <format>
#include <vector>
#include <array>

#include <engine_export.h>

class ENGINE_EXPORT Texture
{
public:
    friend struct std::formatter<Texture>;
protected:
    Texture(uint32_t texType);
    Texture(const Texture&) = delete;
    auto operator=(const Texture&) -> Texture& = delete;
    
    Texture(Texture&&) noexcept;
    auto operator=(Texture&&) noexcept-> Texture&;
    ~Texture();

public:
    auto id() const -> uint32_t;
    auto bind() const -> void;
    auto type() const -> uint32_t;
    auto type_name() const -> std::string;

    static auto gl_format(Image::Format fmt) -> uint32_t;
    static auto gl_internal_format(Image::Format fmt) -> uint32_t;

    static auto storage2d(const auto* data, uint32_t type, int32_t width, int32_t height, int32_t intformat, uint32_t format) -> void;

protected:
    uint32_t m_Id;
    uint32_t m_Type;
};

class ENGINE_EXPORT Texture2D final : public Texture
{
  public:
    Texture2D();
    Texture2D(const Texture2D&) = delete;
    auto operator=(const Texture2D&) -> Texture2D& = delete;

    Texture2D(const char* name);
    Texture2D(auto* data, int32_t width, int32_t height, Image::Format fmt);

  private:
    Image m_Img;
};

class ENGINE_EXPORT TextureCubeMap final : public Texture
{
  public:
    TextureCubeMap();
    TextureCubeMap(const TextureCubeMap&) = delete;
    auto operator=(const TextureCubeMap&) -> TextureCubeMap& = delete;

    TextureCubeMap(const std::vector<std::string> faces);

    static auto base_to_6faces(const std::string& path) -> std::vector<std::string>;

  private:
    std::array<Image, 6> m_Imgs;
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