#pragma once
#include <engine_export.h>

#include <emath/vec2.hpp>
#include <emath/ivec2.hpp>
#include <emath/u16vec4.hpp>
#include <emath/hash.hpp>

#include <string>
#include <unordered_map>
#include <vector>
#include <array>
#include <cstdint>

#include "UiFont.hpp"

class ENGINE_EXPORT Text {
public:
    Text();

    struct Glyph {
        emath::vec2 offset;   // world position of the bottom‑left of the glyph
        emath::u16vec4 texRect;  // (uMin, vMin, uMax, vMax) in atlas
    };

    auto text(std::string text, emath::vec2 pos) -> void;   
    auto draw(std::string text) -> void;
    auto bitmap(int32_t w, int32_t h) -> std::vector<uint8_t>;
    auto atlas_dims() const -> std::pair<int32_t, int32_t>;
    auto fill_text_buffer(int32_t width, int32_t height) -> void;
    auto clear() -> void;
    auto clear_glyphs() -> void;
    auto glyphs() const -> const std::vector<Glyph>&;

public:
    constexpr static const char* DEFAULT_FONT_NAME = "res/JetBrainsMonoNL-BoldItalic.ttf";
    constexpr static float DEFAULT_FONT_SIZE = 24.0f;
    inline    static emath::vec3 DEFAULT_FONT_COLOR = {139.0f, 0.0f, 0.0f};

    constexpr static uint8_t FIRST_GLYPH = 32;
    constexpr static uint8_t LAST_GLYPH = 127;
    constexpr static uint32_t GLYPH_COUNT = LAST_GLYPH - FIRST_GLYPH + 1;

private:
    UiFont m_Font;
    emath::vec2 m_Cursor;

    std::unordered_map<emath::vec2, std::string> m_Text;
    std::array<UiFont::bakedchar, GLYPH_COUNT> m_Glyphs;
    std::vector<Glyph> m_TextGlyphs;
};