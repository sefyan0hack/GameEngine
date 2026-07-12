#include "Text.hpp"
#include "UiFont.hpp"

#include <core/Exception.hpp>

#include <string>
#include <cmath>
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
    this->text(std::move(text), m_Cursor);

    m_Cursor.x = 0.0f;
    m_Cursor.y += DEFAULT_FONT_SIZE;
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

auto Text::fill_text_buffer(int32_t width, int32_t height) -> void
{
    if (m_Text.empty()) return;

    auto scale = m_Font.pixel_scale(DEFAULT_FONT_SIZE);
    auto ascent = m_Font.ascent();
    auto descent = m_Font.descent();
    auto linegap = m_Font.linegap();

    m_Cursor = emath::vec2(0.0f);
    for (const auto& [pos, text] : m_Text) {
        float start_x = pos.x;
        float x = start_x;
        float y = static_cast<float>(height) - pos.y - (ascent * scale);

        for (auto c : text) {

            uint8_t ch = static_cast<uint8_t>(c);
            const auto& glyph =
                (ch >= FIRST_GLYPH && ch <= LAST_GLYPH)
                    ? m_Glyphs[ch - FIRST_GLYPH]
                    : m_Glyphs['?' - FIRST_GLYPH];

            auto line_advance = [&](){ x = start_x; y -= (ascent - descent + linegap) * scale; };
            auto glyph_advance = [&](int n = 1){  x += glyph.xadvance *  n; };

            switch(c)
            {
                case '\n': line_advance(); continue;
                case '\t': glyph_advance(4); continue;
                case ' ' : glyph_advance(); continue;
            }

            float w = static_cast<float>(glyph.x1 - glyph.x0);
            float h = static_cast<float>(glyph.y1 - glyph.y0);

            if (int32_t(x + w) >= width) line_advance();

            float xpos = x + glyph.xoff;
            float ypos = y - glyph.yoff - h;

            m_TextGlyphs.push_back(Glyph {
                .offset = {xpos, ypos},
                .texRect = {glyph.x0, glyph.y0, glyph.x1, glyph.y1}
            });

            glyph_advance();
        }
    }

    if (m_TextGlyphs.empty()) return;
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