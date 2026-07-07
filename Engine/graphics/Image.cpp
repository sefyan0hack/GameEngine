#include <cstring>

#include "Image.hpp"
#include <core/Log.hpp>
#include <core/Exception.hpp>
#include <bit>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_THREAD_LOCALS
#include <stb/stb_image.h>

namespace eg_detail {
    constexpr auto floorf(float x) -> float
    {
        int i = static_cast<int>(x);
        // if x is negative and not already an integer → step down
        if (x < 0.0f && x != static_cast<float>(i))
            return static_cast<float>(i - 1);
        return static_cast<float>(i);
    };
    constexpr auto fmodf(float x, float y) -> float
    {
        // assume y != 0
        float n = x / y;
        int i = static_cast<int>(n);
        float r = x - static_cast<float>(i) * y;
        return r;
    };
}

constexpr std::size_t procedural_texture_width  = 64;
constexpr std::size_t procedural_texture_height = 64;
constexpr std::size_t procedural_texture_channels = sizeof(uint32_t);

constexpr auto procedural_texture_checkerboard = [] {
    std::array<uint8_t, procedural_texture_width * procedural_texture_height * procedural_texture_channels > data{};

    for (uint8_t y = 0; y < procedural_texture_height; ++y) {
        for (uint8_t x = 0; x < procedural_texture_width; ++x) {

            float s = static_cast<float>(x) / (procedural_texture_width - 1);
            float t = static_cast<float>(y) / (procedural_texture_height - 1);

            float pattren = eg_detail::fmodf(
                eg_detail::floorf(10.0f * s) + eg_detail::floorf(10.0f * t),
                2.0f
            );

            size_t offset = (y * procedural_texture_width + x) * procedural_texture_channels;

            uint32_t color = pattren ? 0xFFFF00FF : 0x00000000;

            data[offset]     = static_cast<uint8_t>(color & 0xFF);
            data[offset + 1] = static_cast<uint8_t>((color >> 8) & 0xFF);
            data[offset + 2] = static_cast<uint8_t>((color >> 16) & 0xFF);
            data[offset + 3] = static_cast<uint8_t>((color >> 24) & 0xFF);
        }
    }

    return data;
}();

//default pattren
constexpr auto procedural_texture_func = procedural_texture_checkerboard;

Image::Image()
    : m_Width(procedural_texture_width)
    , m_Height(procedural_texture_height)
    , m_Channels(procedural_texture_channels)
    , m_Data(procedural_texture_func.data())
{
}

Image::Image(auto* Data, uint32_t Width, uint32_t Height, uint32_t Channels)
{
    if(Data != nullptr && Width > 0 && Height > 0 && Channels > 0){
        m_Width = Width;
        m_Height = Height;
        m_Channels = Channels;
        m_Data = std::bit_cast<const uint8_t*>(Data);
    }else{
        m_Width = procedural_texture_width;
        m_Height = procedural_texture_height;
        m_Channels = procedural_texture_channels;
        m_Data = procedural_texture_func.data();

        logg::warn("Can't load raw data");
    }
}

Image::Image(auto* Data, uint32_t Width, uint32_t Height, Format format)
{
    auto channels = std::to_underlying(format);

    if(Data != nullptr && Width > 0 && Height > 0 && channels > 0){
        m_Width = Width;
        m_Height = Height;
        m_Channels = channels;
        m_Data = std::bit_cast<const uint8_t*>(Data);
    }else{
        m_Width = procedural_texture_width;
        m_Height = procedural_texture_height;
        m_Channels = procedural_texture_channels;
        m_Data = procedural_texture_func.data();

        logg::warn("Can't load raw data");
    }
}

Image::Image(const std::string& filename, bool flip)
{
    stbi_set_flip_vertically_on_load(flip); //TODO: revisite .c_str() on const std::string& ?
    auto data = stbi_load(filename.c_str(), &m_Width, &m_Height, &m_Channels, 0);

    if(data){
        m_Data = std::bit_cast<const uint8_t*>(data);
    }
    else
    {
        m_Width = procedural_texture_width;
        m_Height = procedural_texture_height;
        m_Channels = procedural_texture_channels;
        m_Data = procedural_texture_func.data();

        logg::warn("Can't read {} . reason : {}", filename.c_str(), stbi_failure_reason());
    }
}

Image::Image(std::span<const char> src, bool flip)
{
    stbi_set_flip_vertically_on_load(flip);

    auto data = stbi_load_from_memory(std::bit_cast<const unsigned char*>(src.data()), static_cast<int>(src.size()), &m_Width, &m_Height, &m_Channels, 0);

    if(data){
        m_Data = std::bit_cast<const uint8_t*>(data);
    }
    else{
        m_Width = procedural_texture_width;
        m_Height = procedural_texture_height;
        m_Channels = procedural_texture_channels;
        m_Data = procedural_texture_func.data();

        logg::warn("Can't read file . reason : {}", stbi_failure_reason());
    }
}

Image::~Image()
{
    if(m_Data && m_Data != procedural_texture_func.data()) stbi_image_free(const_cast<uint8_t*>(m_Data));
}

Image::Image(Image&& other)
    : m_Width(std::exchange(other.m_Width, 0))
    , m_Height(std::exchange(other.m_Height, 0))
    , m_Channels(std::exchange(other.m_Channels, 0))
    , m_Data(std::exchange(other.m_Data, nullptr))
{
}

auto Image::operator=(Image&& other) -> Image&
{
    if(this != &other){
        this->m_Width = std::exchange(other.m_Width, 0);
        this->m_Height = std::exchange(other.m_Height, 0);
        this->m_Channels = std::exchange(other.m_Channels, 0);
        this->m_Data = std::exchange(other.m_Data, nullptr);
    }
    return *this;
}

auto Image::width() const -> int32_t
{
    return m_Width;
}
auto Image::height() const -> int32_t
{
    return m_Height;
}
auto Image::channels() const -> int32_t
{
    return m_Channels;
}

auto Image::size() const -> std::size_t
{
    return static_cast<std::size_t>(m_Width * m_Height * m_Channels);
}

auto Image::valid() const ->bool
{
    return
        m_Data != nullptr 
        && m_Width > 0
        && m_Height > 0
        && m_Channels > 0;
}


auto Image::data() const -> std::span<const uint8_t>
{
    Expect(valid(), " Image not Valid {:p}", (const void*)this);

    return { m_Data, size() };
}

auto Image::format() const -> Format
{
    Expect(m_Channels > 0 && m_Channels  <= 4, "supported channels count is less then 4");
    return static_cast<Format>(m_Channels);
}
