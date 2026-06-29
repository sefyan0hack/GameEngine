#include "UiFont.hpp"

#include <core/Exception.hpp>
#include <core/res.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

#if defined(min) || defined(max)
#undef min
#undef max
#endif

static_assert(sizeof(UiFont::bakedchar) == sizeof(stbtt_bakedchar));

UiFont::UiFont(const char *name)
    : m_Name(name)
    , m_Handle(new stbtt_fontinfo())
{
    auto font_data = res::get(m_Name);

    if (!stbtt_InitFont(m_Handle, reinterpret_cast<const unsigned char*>(font_data.data()), 0)) {
        throw Exception("Failed to load font {}", m_Name);
    }
}

UiFont::~UiFont(){
    if(m_Handle) delete m_Handle;
}

auto UiFont::name() const -> const char *
{
    return m_Name;
}

auto UiFont::ascent() const -> int32_t
{
    int32_t a{};
    stbtt_GetFontVMetrics(m_Handle, &a, nullptr, nullptr);
    return a;
}

auto UiFont::descent() const -> int32_t
{
    int32_t d{};
    stbtt_GetFontVMetrics(m_Handle, nullptr, &d, nullptr);
    return d;
}

auto UiFont::linegap() const -> int32_t
{
    int32_t l{};
    stbtt_GetFontVMetrics(m_Handle, nullptr, nullptr, &l);
    return l;
}

auto UiFont::pixel_scale(float scale) const -> float
{
    return stbtt_ScaleForPixelHeight(m_Handle, scale);
}

auto UiFont::get_code_point_bitmap_box(int codepoint, float scale_x, float scale_y, int *ix0, int *iy0, int *ix1, int *iy1) const -> void
{
    stbtt_GetCodepointBitmapBox(
        m_Handle,
        codepoint,
        scale_x,
        scale_y,
        ix0, iy0,
        ix1, iy1
    );
}

auto UiFont::bake_font_bitmap(int offset,
    float pixel_height, unsigned char *pixels, int pw, int ph,
    int first_char, int num_chars, bakedchar *chardata
) -> int
{
    return stbtt_BakeFontBitmap(
        reinterpret_cast<const unsigned char*>(res::get(m_Name).data()),
        offset,
        pixel_height,
        pixels, pw, ph,
        first_char, num_chars,
        reinterpret_cast<stbtt_bakedchar*>(chardata)
    );
}