#include <cstring>

#include "Image.hpp"
#include <core/Log.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


template <std::size_t Size>
constexpr auto generate_checkerboard(const uint32_t color1, const uint32_t color2)
{
    constexpr std::size_t bytes_per_pixel = 3;
    constexpr std::size_t total_bytes = Size * Size * bytes_per_pixel;

    std::array<std::byte, total_bytes> data{};

    for (std::size_t y = 0; y < Size; ++y) {
        for (std::size_t x = 0; x < Size; ++x) {

            const bool use_first = ((x / 5 + y / 5) % 2) == 0;
            const uint32_t c = use_first ? color1 : color2;

            const std::size_t idx = (y * Size + x) * bytes_per_pixel;

            for (std::size_t b = 0; b < bytes_per_pixel; ++b) {
                data[idx + b] = static_cast<std::byte>((c >> (8 * b)) & 0xFFu);
            }
        }
    }

    return data;
}

// magenta/black checkerboard 50x50
// constexpr auto checkerboard = generate_checkerboard<50>(0xFFFF00FF, 0x00000000);

Image::Image()
    : m_Width(50), m_Height(50), m_Channels(3)
    , m_Data(static_cast<std::byte*>(STBI_MALLOC(size())))
{
    // magenta/black checkerboard 50x50
    std::memcpy(m_Data, generate_checkerboard<50>(0xFFFF00FF, 0x00000000).data(), size());
}

Image::Image(Pointer auto Data, uint32_t Width, uint32_t Height, uint32_t Channels)
    : Image()
{
    if(Data != nullptr && Width > 0 && Height > 0 && Channels > 0){
        m_Width = Width;
        m_Height = Height;
        m_Channels = Channels;
        m_Data = reinterpret_cast<std::byte*>(Data);
    }else{
        debug::print(" Image not Valid  {}", *this);
    }
}

Image::Image(const std::string& filename, bool flip)
    : Image()
{
    stbi_set_flip_vertically_on_load(flip);
    auto data = stbi_load(filename.c_str(), &m_Width, &m_Height, &m_Channels, 0);

    if(data)
        m_Data = reinterpret_cast<std::byte*>(data);
    else
        debug::print("Can't read {} . reason : {}", filename.c_str(), stbi_failure_reason());
}

Image::Image(const cmrc::file& src, bool flip)

{
    stbi_set_flip_vertically_on_load(flip);
    auto size = std::distance(src.begin(), src.end());

    auto data = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(src.begin()), static_cast<int>(size), &m_Width, &m_Height, &m_Channels, 0);

    if(data)
        m_Data = reinterpret_cast<std::byte*>(data);
    else
        debug::print("Can't read file . reason : {}", stbi_failure_reason());
}


Image::~Image()
{
    if(m_Data) stbi_image_free(m_Data);
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


auto Image::data() const -> std::span<std::byte>
{
    Expect(valid(), " Image not Valid  {}", *this);

    return { m_Data, size() };
}

auto Image::cpu_format() const -> GLenum
{
    switch(m_Channels) {
        case 1: return GL_RED;
        case 2: return GL_RG;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: throw Exception("Unsupported channel count: {}", m_Channels);
    }
}


auto Image::gpu_format() const -> GLint
{
    switch(m_Channels) {
        case 1: return GL_R8;
        case 2: return GL_RG8;
        case 3: return GL_RGB8;
        case 4: return GL_RGBA8;
        default: throw Exception("Unsupported channel count: {}", m_Channels);
    }
}

auto Image::cpu_to_gpu_format(GLenum cpuformat) -> GLint
{
    switch(cpuformat) {
        case GL_RED: return GL_R8;
        case GL_RG: return GL_RG8;
        case GL_RGB: return GL_RGB8;
        case GL_RGBA: return GL_RGBA8;
        default: throw Exception("Unsupported Fromat: {:X}", cpuformat);
    }
}

auto Image::gpu_to_cpu_format(GLint gpuformt) -> GLenum
{
    switch(gpuformt) {
        case GL_R8: return GL_RED;
        case GL_RG8: return GL_RG;
        case GL_RGB8: return GL_RGB;
        case GL_RGBA8: return GL_RGBA;
        default: throw Exception("Unsupported Fromat: {:X}", gpuformt);
    }
}

auto Image::channel_from_cpu_format(GLenum format) -> std::int32_t
{
    switch(format) {
        case GL_R8: return 1;
        case GL_RG8: return 2;
        case GL_RGB8: return 3;
        case GL_RGBA8: return 4;
        default: throw Exception("Unsupported Fromat: {:X}", format);
    }
}

auto Image::channel_from_gpu_format(GLint format) -> std::int32_t
{
    switch(format) {
        case GL_RED: return 1;
        case GL_RG: return 2;
        case GL_RGB: return 3;
        case GL_RGBA: return 4;
        default: throw Exception("Unsupported Fromat: {:X}", format);
    }
}