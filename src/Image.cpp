#include <core/Image.hpp>
#include <core/Log.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


template <std::size_t Size>
constexpr auto generate_checkerboard(const uint32_t color1, const uint32_t color2)
{
    constexpr std::size_t bytes_per_pixel = sizeof(uint32_t);
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

// magenta/black checkerboard 100x100
constexpr auto checkerboard = generate_checkerboard<100>(0xFFFF00FF, 0x00000000);

Image::Image()
    : m_Width(100), m_Height(100), m_Channels(sizeof(uint32_t))
    , m_Data(const_cast<std::byte*>(checkerboard.data()))
{
}

Image::Image(Pointer auto Data, uint32_t Width, uint32_t Height, uint32_t Channels)
    : m_Width(Width), m_Height(Height), m_Channels(Channels)
    , m_Data(reinterpret_cast<std::byte*>(Data))
{
    if(!Valid()) Error(" Image not Valid  {}", *this);
}

Image::Image(const std::string& filename, bool flip)
    : Image()
{
    stbi_set_flip_vertically_on_load(flip);
    auto data = stbi_load(filename.c_str(), &m_Width, &m_Height, &m_Channels, 0);

    if(data)
        m_Data = reinterpret_cast<std::byte*>(data);
    else
        Info("Can't read {} . reason : {}", filename.c_str(), stbi_failure_reason());
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

auto Image::Width() const -> int32_t
{
    return m_Width;
}
auto Image::Height() const -> int32_t
{
    return m_Height;
}
auto Image::Channels() const -> int32_t
{
    return m_Channels;
}

auto Image::Valid() const ->bool
{
    return
        m_Data != nullptr 
        && m_Width > 0
        && m_Height > 0
        && m_Channels > 0;
}


auto Image::Data() const -> std::span<std::byte>
{
    if(!Valid()) Error(" Image not Valid  {}", *this);

    return { m_Data, m_Width * m_Height * m_Channels};
}

