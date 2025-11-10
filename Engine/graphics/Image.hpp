#pragma once

#include <string>
#include <format>
#include <span>

#include "gl.h"
#include <cmrc/cmrc.hpp>
#include <core/Utils.hpp>
#include <engine_export.h>

class ENGINE_API Image
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

    auto width() const -> int32_t;
    auto height() const -> int32_t;
    auto channels() const -> int32_t;

    auto cpu_format() const -> GLenum;
    auto gpu_format() const -> GLint;

    static auto cpu_to_gpu_format(GLenum cpuformat) -> GLint;
    static auto gpu_to_cpu_format(GLint gpuformt) -> GLenum;

    static auto channel_from_cpu_format(GLenum format) -> std::int32_t;
    static auto channel_from_gpu_format(GLint format) -> std::int32_t;

    auto data() const -> std::span<std::byte>;
    auto size() const -> std::size_t;
    auto valid() const -> bool;

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