#include "Text.hpp"
#include "UiFont.hpp"
#include "gl.hpp"

#include <core/Exception.hpp>
#include <core/res.hpp>

#include <graphics/Window.hpp>
#include <graphics/OpenGL.hpp>
#include <graphics/Shader.hpp>
#include <graphics/ShaderProgram.hpp>

#include <emath/emath.hpp>

#include <algorithm>
#include <string>
#include <cstring>
#include <cmath>
#include <bit>

#if defined(min) || defined(max)
#undef min
#undef max
#endif


Text::Text(const OpenGL& ctx)
    : m_GApi(ctx)
    , m_Program(std::make_shared<ShaderProgram>("res/Shaders/text.vert", "res/Shaders/text.frag"))
    , m_Font(DEFAULT_FONT_NAME)
    , m_Cursor(emath::vec2(0.0f))
    , VAO(0), VBO(0), m_Atlas(0)
{
    m_Instances.reserve(BATCH_SIZE);
    // Initialize buffers
    prepare_buffers();

    gl::GenTextures(1, &m_Atlas);
    gl::BindTexture(GL_TEXTURE_2D, m_Atlas);

    auto [w, h] = atlas_dims();
    std::vector<uint8_t> bitmap(w*h, 0);

    auto result = m_Font.bake_font_bitmap(
        0,
        DEFAULT_FONT_SIZE,
        bitmap.data(), w, h,
        FIRST_GLYPH, GLYPH_COUNT,
        m_Glyphs.data()
    );

    if (result <= 0) {
        throw Exception("Failed to bake font bitmap");
    }

    gl::TexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.data());

    // Set texture parameters
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Text::~Text() {
    gl::DeleteTextures(1, &m_Atlas);
    gl::DeleteBuffers(1, &VBO);
    gl::DeleteVertexArrays(1, &VAO);
}

auto Text::prepare_buffers() -> void {
    // Generate and bind VAO
    gl::GenVertexArrays(1, &VAO);
    gl::BindVertexArray(VAO);

    // Dynamic instance VBO
    gl::GenBuffers(1, &VBO);

    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
    gl::BufferData(GL_ARRAY_BUFFER, BATCH_SIZE * sizeof(Glyph), nullptr, GL_STREAM_DRAW);

    // Offset (2 * 4 byte)
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Glyph), (void*)offsetof(Glyph, offset));
    gl::VertexAttribDivisor(0, 1);

    // TexRect (4 * 2 byte)
    gl::EnableVertexAttribArray(1);
    gl::VertexAttribIPointer(1, 4, GL_UNSIGNED_SHORT, sizeof(Glyph), (void*)offsetof(Glyph, texRect));
    gl::VertexAttribDivisor(1, 1);
}

auto Text::render() -> void {
    if (m_Text.empty()) return;

    auto [width, height] = m_GApi.window().dims();

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

            m_Instances.push_back(Glyph {
                .offset = {xpos, ypos},
                .texRect = {glyph.x0, glyph.y0, glyph.x1, glyph.y1}
            });

            glyph_advance();
        }
    }

    if (m_Instances.empty()) return;

    m_Program->use();

    uint32_t u_ScreenSize = uint32_t(width) | (uint32_t(height) << 16);
    m_Program->set_uniform("u_ScreenSize", u_ScreenSize);

    auto r = uint8_t(std::clamp(DEFAULT_FONT_COLOR.x, 0.0f, 1.0f) * 255.0f + 0.5f);
    auto g = uint8_t(std::clamp(DEFAULT_FONT_COLOR.y, 0.0f, 1.0f) * 255.0f + 0.5f);
    auto b = uint8_t(std::clamp(DEFAULT_FONT_COLOR.z, 0.0f, 1.0f) * 255.0f + 0.5f);

    auto color =(uint32_t(r)      ) |
                (uint32_t(g) << 8 ) |
                (uint32_t(b) << 16) |
                (uint32_t(255) << 24);

    m_Program->set_uniform("u_Color", color);

    gl::ActiveTexture(GL_TEXTURE0);
    gl::BindTexture(GL_TEXTURE_2D, m_Atlas);
    m_Program->set_uniform("u_Texture", 0);

    gl::BindVertexArray(VAO);
    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);

    // gl::DepthFunc(GL_ALWAYS);

    for (size_t offset = 0; offset < m_Instances.size(); offset += BATCH_SIZE)
    {
        auto batchCount = std::min(BATCH_SIZE, m_Instances.size() - offset);
        GLsizeiptr bytesToCopy = batchCount * sizeof(Glyph);

        void* mappedMemory = gl::MapBufferRange(GL_ARRAY_BUFFER, 0, bytesToCopy, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        if (mappedMemory) {
            std::memcpy(mappedMemory, m_Instances.data() + offset, bytesToCopy);
            gl::UnmapBuffer(GL_ARRAY_BUFFER);
            gl::DrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizei>(batchCount));
        }
    }

    // gl::DepthFunc(GL_LESS);

    m_Instances.clear();
    m_Text.clear();
}

auto Text::text(std::string text, emath::vec2 pos) -> void {
    m_Text[pos] = std::move(text);
}

auto Text::draw(std::string text) -> void {
    this->text(std::move(text), m_Cursor);

    m_Cursor.x = 0.0f;
    m_Cursor.y += DEFAULT_FONT_SIZE;
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