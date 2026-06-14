#include "Font.hpp"

#include <graphics/gl.hpp>

#include <core/Exception.hpp>
#include <core/res.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

#include <algorithm>
#include <cmath>
#include <bit>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif


Font::Font(const char *name, float size)
    : m_Name(name)
    , m_Size(size)
    , m_Handle(new stbtt_fontinfo())
    , m_AtlasId(0)
{

    auto font_data = res::get(m_Name);

    if (!stbtt_InitFont(m_Handle, reinterpret_cast<const unsigned char*>(font_data.data()), 0)) {
        throw Exception("Failed to load font {}", m_Name);
    }

    gl::GenTextures(1, &m_AtlasId);
    gl::BindTexture(GL_TEXTURE_2D, m_AtlasId);

    auto [w, h] = atlas_dims();
    auto bitmap = new uint8_t[w*h];
    
    auto result = stbtt_BakeFontBitmap(
        reinterpret_cast<const unsigned char*>(font_data.data()), 0,
        m_Size,
        bitmap, w, h,
        FIRST_GLYPH, GLYPH_COUNT,
        glyphs().data()
    );

    if (result <= 0) {
        throw Exception("Failed to bake font bitmap");
    }

    gl::TexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

    // Set texture parameters
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] bitmap;
}

Font::~Font(){
    if(m_Handle) delete m_Handle;
}

auto Font::name() const -> const char *
{
    return m_Name;
}

auto Font::size() const -> float
{
    return m_Size;
}

auto Font::ascent() const -> int32_t
{
    int32_t a{};
    stbtt_GetFontVMetrics(m_Handle, &a, nullptr, nullptr);
    return a;
}

auto Font::descent() const -> int32_t
{
    int32_t d{};
    stbtt_GetFontVMetrics(m_Handle, nullptr, &d, nullptr);
    return d;
}

auto Font::linegap() const -> int32_t
{
    int32_t l{};
    stbtt_GetFontVMetrics(m_Handle, nullptr, nullptr, &l);
    return l;
}

auto Font::scale() const -> float
{
    return stbtt_ScaleForPixelHeight(m_Handle, m_Size);
}

auto Font::atlas_dims() const -> std::pair<int32_t, int32_t>
{
    int32_t maxW = 0;
    int32_t maxH = 0;

    float s = scale();

    for (int c = FIRST_GLYPH; c <= LAST_GLYPH; ++c) {
        int32_t x0{}, y0{}, x1{}, y1{};

        stbtt_GetCodepointBitmapBox(
            m_Handle,
            c,
            s,
            s,
            &x0, &y0,
            &x1, &y1
        );

        auto w = x1 - x0;
        auto h = y1 - y0;

        maxW = std::max(maxW, w);
        maxH = std::max(maxH, h);
    }

    // grid
    uint32_t cols = (uint32_t)std::ceil(std::sqrt(GLYPH_COUNT));
    uint32_t rows = (uint32_t)std::ceil((float)GLYPH_COUNT / cols);

    // raw atlas size
    uint32_t raw_width  = cols * maxW;
    uint32_t raw_height = rows * maxH;

    // power of 2
    uint32_t atlas_width  = std::bit_ceil(raw_width);
    uint32_t atlas_height = std::bit_ceil(raw_height);

    return {atlas_width, atlas_height};
}

auto Font::atlas_id() const -> uint32_t
{
    return m_AtlasId;
}

auto Font::glyphs() -> std::array<stbtt_bakedchar, Font::GLYPH_COUNT>&
{
    static std::array<stbtt_bakedchar, Font::GLYPH_COUNT> _;
    return _;
}