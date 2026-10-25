#include "Text.hpp"
#include "UiFont.hpp"

#include <core/Exception.hpp>

#include <string>
#include <cmath>
#include <algorithm>
#include <bit>

Text::Text()
    : m_Font(DEFAULT_FONT_NAME)
    , m_Cursor(emath::vec2(0.0f))
{
}

auto Text::text(std::string text, emath::vec2 pos) -> void {
    m_Text[pos] = std::move(text);
}

auto Text::draw(std::string text) -> void {
    auto lines = 1 + std::count(text.begin(), text.end(), '\n');
    this->text(std::move(text), m_Cursor);

    float lineHeight = (m_Font.ascent() - m_Font.descent() + m_Font.linegap()) * m_Font.pixel_scale(DEFAULT_FONT_SIZE);

    m_Cursor.x = 0.0f;
    m_Cursor.y += lineHeight * lines;
}

auto Text::bitmap(int32_t w, int32_t h) -> std::vector<uint8_t>
{
    std::vector<uint8_t> bitmap(w*h, 0);

    auto result = m_Font.bake_font_bitmap(
        0,
        Text::DEFAULT_FONT_SIZE,
        bitmap.data(), w, h,
        FIRST_GLYPH, GLYPH_COUNT,
        m_Glyphs.data()
    );

    if (result <= 0) {
        throw Exception("Failed to bake font bitmap");
    }

    return bitmap;
}

auto Text::fill_text_buffer() -> void
{
    if (m_Text.empty()) return;

    auto scale = m_Font.pixel_scale(DEFAULT_FONT_SIZE);
    auto ascent = m_Font.ascent();
    auto descent = m_Font.descent();
    auto linegap = m_Font.linegap();

    m_Cursor = emath::vec2(0.0f);
    m_TextGlyphs.clear();

    for (const auto& [pos, text] : m_Text) {
        float start_x = pos.x;
        float x = start_x;
        float y = pos.y + (ascent * scale);

        for (auto c : text) {

            uint8_t ch = static_cast<uint8_t>(c);
            const auto& glyph =
                (ch >= FIRST_GLYPH && ch <= LAST_GLYPH)
                    ? m_Glyphs[ch - FIRST_GLYPH]
                    : m_Glyphs['?' - FIRST_GLYPH];

            auto line_advance = [&](){ x = start_x; y += (ascent - descent + linegap) * scale; };
            auto glyph_advance = [&](int n = 1){  x += glyph.xadvance * n; };

            switch(c)
            {
                case '\n': line_advance(); continue;
                case '\t': glyph_advance(4); continue;
                case ' ' : glyph_advance(); continue;
            }

            m_TextGlyphs.push_back(Glyph {
                .position = {x + glyph.xoff, y + glyph.yoff},
                .uv_rect = {glyph.x0, glyph.y0, glyph.x1, glyph.y1}
            });

            glyph_advance();
        }
    }
}

auto Text::glyphs() const -> const std::vector<Glyph>&
{
    return m_TextGlyphs;
}

auto Text::clear_glyphs() -> void
{
    m_TextGlyphs.clear();
}

auto Text::clear() -> void
{
    m_Text.clear();
}


auto Text::atlas_dims() const -> std::pair<int32_t, int32_t>
{
    int32_t maxW = 0;
    int32_t maxH = 0;

    float s = m_Font.pixel_scale(DEFAULT_FONT_SIZE);

    for (int c = FIRST_GLYPH; c <= LAST_GLYPH; ++c) {
        int32_t x0{}, y0{}, x1{}, y1{};

        m_Font.get_code_point_bitmap_box(c, s, s, &x0, &y0, &x1, &y1);

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

auto Text::text_width(const std::string& text) const -> float
{
    float x = 0.0f;
    float max_width = 0.0f;

    for (char c : text)
    {
        const auto ch = static_cast<uint8_t>(c);

        const auto& glyph =
            (ch >= FIRST_GLYPH && ch <= LAST_GLYPH)
                ? m_Glyphs[ch - FIRST_GLYPH]
                : m_Glyphs['?' - FIRST_GLYPH];

        switch (c)
        {
            case '\n': x = 0.0f; break;
            case '\t': x += glyph.xadvance * 4.0f; break;
            default:   x += glyph.xadvance;        break;
        }
    }

    return std::max(max_width, x);
}

auto Text::text_height(const std::string& text) const -> float
{
    const float scale = m_Font.pixel_scale(DEFAULT_FONT_SIZE);

    const auto lines = 1 + std::count(text.begin(), text.end(), '\n');

    return lines * (m_Font.ascent() - m_Font.descent() + m_Font.linegap()) * scale;
}