#pragma once

#include <string>
#include <format>
#include <cstdint>
#include <span>

#include <core/res.hpp>
#include <engine_export.h>

class ENGINE_EXPORT Image
{
public:
    enum class Format {
      RED   = 1,
      RG    = 2,
      RGB   = 3,
      RGBA  = 4
    };

    friend struct std::formatter<Image>;

    Image();
    Image(const char* filename, bool flip = false);
    Image(auto* Data, uint32_t Width, uint32_t Height, uint32_t Channels);
    Image(auto* Data, uint32_t Width, uint32_t Height, Format format);
    ~Image();

    Image(const Image& other) = delete;
    auto operator=(const Image& other) -> Image& = delete;

    Image(Image&& other);
    auto operator=(Image&& other) -> Image&;

    auto width() const -> int32_t;
    auto height() const -> int32_t;
    auto channels() const -> int32_t;
    auto format() const -> Format;

    auto data() const -> std::span<const uint8_t>;
    auto size() const -> std::size_t;
    auto valid() const -> bool;

private:
    int32_t m_Width, m_Height, m_Channels;
    const uint8_t* m_Data;
};

#ifdef __cpp_lib_formatters
// custom Image Format
template<>
struct std::formatter<Image> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Image& obj, auto& context) const {
    return std::format_to(context.out(),
    R"({{ "Width": {}, "Height": {}, "Channels": {}, "Data": {} }})"
    , obj.m_Width, obj.m_Height, obj.channels(), static_cast<const void*>(obj.m_Data));
  }
};
#endif