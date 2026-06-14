#include "Text.hpp"
#include "Font.hpp"
#include "gl.hpp"

#include <core/Exception.hpp>
#include <core/res.hpp>

#include <graphics/Window.hpp>
#include <graphics/OpenGL.hpp>
#include <graphics/Shader.hpp>
#include <graphics/ShaderProgram.hpp>

#include <emath/emath.hpp>

#include <string>
#include <vector>

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
    gl::BufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(Font::Glyph), nullptr, GL_DYNAMIC_DRAW);

    // Offset (2 * 4 byte)
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Font::Glyph), (void*)offsetof(Font::Glyph, offset));
    gl::VertexAttribDivisor(0, 1);

    // TexRect (4 * 2 byte)
    gl::EnableVertexAttribArray(1);
    gl::VertexAttribIPointer(1, 4, GL_UNSIGNED_SHORT, sizeof(Font::Glyph), (void*)offsetof(Font::Glyph, texRect));
    gl::VertexAttribDivisor(1, 1);

    gl::BindVertexArray(0);
}


auto Text::render() -> void {
    if (m_Text.empty()) return;

    auto [width, height] = m_GApi.window().dims();
    auto projection = emath::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

    std::vector<Font::Glyph> instances;
    instances.reserve(256);

    auto scale = m_Font.scale();
    auto ascent = m_Font.ascent();
    auto descent = m_Font.descent();
    auto linegap = m_Font.linegap();

    float scaledlineHeight = (scale - descent + linegap) * scale;

    m_Cursor = emath::vec2(0.0f);
    for (const auto& [pos, text] : m_Text) {
        float startX = pos.x;
        float x = startX;
        float y = static_cast<float>(height) - pos.y - (ascent * scale);

        for (auto c : text) {
            uint8_t ch = static_cast<uint8_t>(c);

            const auto& glyph =
                (ch >= Font::FIRST_GLYPH && ch <= Font::LAST_GLYPH)
                    ? Font::glyphs()[ch - Font::FIRST_GLYPH]
                    : Font::glyphs()['?' - Font::FIRST_GLYPH];

            float w = static_cast<float>(glyph.x1 - glyph.x0);
            float h = static_cast<float>(glyph.y1 - glyph.y0);

            if ((x + w) >= static_cast<float>(width)) {
                x = startX;
                y -= scaledlineHeight;
            }

            float xpos = x + glyph.xoff;
            float ypos = y - glyph.yoff - h;

            Font::Glyph ins { .offset = {xpos, ypos} };
            std::memcpy(&ins.texRect, &glyph.x0, 4 * sizeof(uint16_t));
            instances.push_back(ins);

            x += glyph.xadvance;
        }
    }

    if (instances.empty()) return;

    // Upload instance data
    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
    gl::BufferSubData(GL_ARRAY_BUFFER, 0, instances.size() * sizeof(Font::Glyph), instances.data());

    gl::DepthMask(GL_FALSE);
    gl::DepthFunc(GL_ALWAYS);

    m_Program->use();
    m_Program->set_uniform("u_Projection", projection);
    m_Program->set_uniform("u_Color", FONT_COLOR);

    gl::ActiveTexture(GL_TEXTURE0);
    gl::BindTexture(GL_TEXTURE_2D, m_Font.atlas_id());

    gl::BindVertexArray(VAO);

    gl::DrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, static_cast<GLsizei>(instances.size()));

    gl::DepthFunc(GL_LEQUAL);
    gl::DepthMask(GL_TRUE);
}

auto Text::text(std::string text, emath::vec2 pos) -> void {
    m_Text[pos] = std::move(text);
}

auto Text::draw(std::string text) -> void {
    this->text(std::move(text), m_Cursor);

    m_Cursor.x = 0.0f;
    m_Cursor.y += m_Font.size();
}