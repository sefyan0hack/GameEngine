#pragma once

#include <string>
#include <format>
#include <span>

#include "gl.h"
#include <cmrc/cmrc.hpp>

class Image
{
public:
    friend struct std::formatter<Image>;

    Image();
    Image(const std::string& filename, bool flip = false);
    Image(const cmrc::file& src, bool flip = false);
    Image(Pointer auto Data, uint32_t Width, uint32_t Height, uint32_t Channels);
    ~Image();

    Image(const Image& other) = delete;
    auto operator=(const Image& other) -> Image& = delete;

    Image(Image&& other);
    auto operator=(Image&& other) -> Image&;

    auto Width() const -> int32_t;
    auto Height() const -> int32_t;
    auto Channels() const -> int32_t;

    auto CPUFormat() const -> GLenum;
    auto GPUFormat() const -> GLint;

    static auto CPUtoCGPUFormat(GLenum cpuformat) -> GLint;
    static auto GPUtoCPUFormat(GLint gpuformt) -> GLenum;

    static auto ChannelFromCPUFormat(GLenum format) -> std::int32_t;
    static auto ChannelFromGPUFormat(GLint format) -> std::int32_t;

    auto Data() const -> std::span<std::byte>;
    auto Size() const -> std::size_t;
    auto Valid() const -> bool;

private:
    int32_t m_Width, m_Height, m_Channels;
    std::byte* m_Data;
};

// custom Image Format
template<>
struct std::formatter<Image> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Image& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "Width": {}, "Height": {}, "Channels": {}, "Data": {} }})"
    , obj.m_Width, obj.m_Height, obj.m_Channels, static_cast<const void*>(obj.m_Data));
  }
};