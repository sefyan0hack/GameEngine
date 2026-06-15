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
#include <vector>
#include <cstring>

#if defined(min) || defined(max)
#undef min
#undef max
#endif

Text::Text(const OpenGL& ctx)
    : m_GApi(ctx)
    , m_Vert(std::make_shared<Shader>(res::get("res/Shaders/text.vert"), GL_VERTEX_SHADER))
    , m_Frag(std::make_shared<Shader>(res::get("res/Shaders/text.frag"), GL_FRAGMENT_SHADER))
    , m_Program(std::make_shared<ShaderProgram>(m_Vert, m_Frag))
    , m_Font(DEFAULT_FONT_NAME)
    , m_Cursor(emath::vec2(0.0f))
    , VAO(0), VBO(0)
{
    // Initialize buffers
    prepare_buffers();
}

Text::~Text() {
    gl::DeleteVertexArrays(1, &VAO);
    gl::DeleteBuffers(1, &VBO);
}

auto Text::prepare_buffers() -> void {
    // Generate and bind VAO
    gl::GenVertexArrays(1, &VAO);
    gl::BindVertexArray(VAO);

    // --- Dynamic instance VBO (attribute 2..5) ---
    gl::GenBuffers(1, &VBO);

    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
    gl::BufferData(GL_ARRAY_BUFFER, BATCH_SIZE * sizeof(UiFont::Glyph), nullptr, GL_DYNAMIC_DRAW);

    // Offset (2 * 4 byte)
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UiFont::Glyph), (void*)offsetof(UiFont::Glyph, offset));
    gl::VertexAttribDivisor(0, 1);

    // TexRect (4 * 2 byte)
    gl::EnableVertexAttribArray(1);
    gl::VertexAttribIPointer(1, 4, GL_UNSIGNED_SHORT, sizeof(UiFont::Glyph), (void*)offsetof(UiFont::Glyph, texRect));
    gl::VertexAttribDivisor(1, 1);

    gl::BindVertexArray(0);
}


auto Text::render() -> void {
    if (m_Text.empty()) return;

    auto [width, height] = m_GApi.window().dims();

    auto scale = m_Font.scale();
    auto ascent = m_Font.ascent();
    auto descent = m_Font.descent();
    auto linegap = m_Font.linegap();

    float scaledlineHeight = (ascent - descent + linegap) * scale;

    m_Cursor = emath::vec2(0.0f);
    for (const auto& [pos, text] : m_Text) {
        float startX = pos.x;
        float x = startX;
        float y = static_cast<float>(height) - pos.y - (ascent * scale);

        for (auto c : text) {
            uint8_t ch = static_cast<uint8_t>(c);

            const auto& glyph =
                (ch >= UiFont::FIRST_GLYPH && ch <= UiFont::LAST_GLYPH)
                    ? UiFont::glyphs()[ch - UiFont::FIRST_GLYPH]
                    : UiFont::glyphs()['?' - UiFont::FIRST_GLYPH];

            float w = static_cast<float>(glyph.x1 - glyph.x0);
            float h = static_cast<float>(glyph.y1 - glyph.y0);

            if ((x + w) >= static_cast<float>(width)) {
                x = startX;
                y -= scaledlineHeight;
            }

            float xpos = x + glyph.xoff;
            float ypos = y - glyph.yoff - h;

            UiFont::Glyph ins { .offset = {xpos, ypos} };
            std::memcpy(&ins.texRect, &glyph.x0, 4 * sizeof(uint16_t));
            m_Instances.push_back(ins);

            x += glyph.xadvance;
        }
    }

    if (m_Instances.empty()) return;
    
    m_Program->use();

    auto u_dims = emath::vec2(static_cast<float>(width), static_cast<float>(height));
    m_Program->set_uniform("u_Dims", u_dims);

    auto r = uint8_t(std::clamp(FONT_COLOR.x, 0.0f, 1.0f) * 255.0f + 0.5f);
    auto g = uint8_t(std::clamp(FONT_COLOR.y, 0.0f, 1.0f) * 255.0f + 0.5f);
    auto b = uint8_t(std::clamp(FONT_COLOR.z, 0.0f, 1.0f) * 255.0f + 0.5f);

    auto color =(uint32_t(r)      ) |
                (uint32_t(g) << 8 ) |
                (uint32_t(b) << 16) |
                (uint32_t(255) << 24);

    m_Program->set_uniform("u_Color", color);

    gl::ActiveTexture(GL_TEXTURE0);
    gl::BindTexture(GL_TEXTURE_2D, m_Font.atlas_id());

    gl::BindVertexArray(VAO);
    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);

    gl::DepthMask(GL_FALSE);
    gl::DepthFunc(GL_ALWAYS);

    for (size_t offset = 0; offset < m_Instances.size(); offset += BATCH_SIZE)
    {
        auto batchCount = std::min(BATCH_SIZE, m_Instances.size() - offset);
        gl::BufferSubData(GL_ARRAY_BUFFER, 0, batchCount * sizeof(UiFont::Glyph), m_Instances.data() + offset);
        gl::DrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, static_cast<GLsizei>(batchCount));
    }

    gl::DepthFunc(GL_LEQUAL);
    gl::DepthMask(GL_TRUE);

    m_Instances.clear();
}

auto Text::text(std::string text, emath::vec2 pos) -> void {
    m_Text[pos] = std::move(text);
}

auto Text::draw(std::string text) -> void {
    this->text(std::move(text), m_Cursor);

    m_Cursor.x = 0.0f;
    m_Cursor.y += m_Font.size();
}