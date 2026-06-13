#pragma once

#include <engine_export.h>

#include <emath/vec2.hpp>
#include <emath/ivec2.hpp>
#include <emath/hash.hpp>

#include <string>
#include <memory>
#include <unordered_map>
#include <array>
#include <cstdint>
#include <bit>

class ENGINE_EXPORT Text {
public:
    Text(const class OpenGL& ctx);
    ~Text();
    
    auto render() -> void;
    auto text(std::string text, emath::vec2 pos) -> void;    

public:
    constexpr static const char* FONT_NAME = "res/JetBrainsMonoNL-BoldItalic.ttf";
    inline    static emath::vec3 FONT_COLOR = {0.1f, 0.85f, 0.1f};
    constexpr static uint32_t FONT_SIZE = 24;

    constexpr static uint32_t GLYPH_PADDING = 1;

    constexpr static uint8_t FIRST_GLYPH = 32;
    constexpr static uint8_t LAST_GLYPH = 127;

    constexpr static uint32_t CHAR_COUNT = LAST_GLYPH - FIRST_GLYPH + 1;
    constexpr static uint32_t CELL_SIZE = FONT_SIZE + (GLYPH_PADDING * 2);

    constexpr static uint32_t ATLAS_COLS = std::bit_ceil(1u << ((std::bit_width(CHAR_COUNT - 1)) / 2));
    constexpr static uint32_t ATLAS_ROWS = (CHAR_COUNT + ATLAS_COLS - 1) / ATLAS_COLS;

    constexpr static uint32_t ATLAS_WIDTH  = std::bit_ceil(ATLAS_COLS * CELL_SIZE);
    constexpr static uint32_t ATLAS_HEIGHT = std::bit_ceil(ATLAS_ROWS * CELL_SIZE);

    static constexpr uint32_t MAX_INSTANCES = 4096;

private:

    struct GlyphInstance {
        emath::vec2 offset;   // world position of the bottom‑left of the glyph
        emath::vec2 size;     // width, height of the glyph
        emath::vec4 texRect;  // (uMin, vMin, uMax, vMax) in atlas
    };

    struct AtlasGlyph {
        emath::vec2 offset;        // Offset from baseline to left/top of glyph
        emath::vec2 size;          // Size of glyph in pixels
        emath::vec4 textRec;        // texture coordinates (Top-left , Bottom-right) in atlas
        uint32_t advance;           // Offset to advance to next glyph (in 1/64 pixels)
    };

    auto prepare_buffers() -> void;
    auto create_atlas() -> void;

    static auto glyphs() -> std::array<AtlasGlyph, CHAR_COUNT>&;

private:
    const class OpenGL& m_GApi;
    std::shared_ptr<class Shader> m_Vert, m_Frag;
    std::shared_ptr<class ShaderProgram> m_Program;

    uint32_t VAO, VBO;
    uint32_t m_AtlasTexture;

    std::unordered_map<emath::vec2, std::string> m_Text;
};