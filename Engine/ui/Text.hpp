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
    inline    static emath::vec3 FONT_COLOR = {139.0f, 0.0f, 0.0f};
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

    struct Glyph {
        emath::vec2 offset;   // world position of the bottom‑left of the glyph
        uint16_t texRect[4];  // (uMin, vMin, uMax, vMax) in atlas
    };

    auto prepare_buffers() -> void;
    auto create_atlas() -> void;

private:
    const class OpenGL& m_GApi;
    std::shared_ptr<class Shader> m_Vert, m_Frag;
    std::shared_ptr<class ShaderProgram> m_Program;

    uint32_t VAO, VBO;
    uint32_t m_AtlasTexture;

    int32_t m_Ascent, m_Descent, m_LineGap;
    float m_Scale;

    std::unordered_map<emath::vec2, std::string> m_Text;
};