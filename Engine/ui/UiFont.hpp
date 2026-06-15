#pragma once
#include <cstdint>
#include <utility>
#include <array>

#include <emath/vec2.hpp>
#include <stb/stb_truetype.h>

class UiFont {
public:
    UiFont(const char* name, float size = 24.0f);
    ~UiFont();

    auto name() const -> const char*;
    auto size() const -> float;
    auto ascent() const -> int32_t;
    auto descent() const -> int32_t;
    auto linegap() const -> int32_t;
    auto scale() const -> float;
    auto atlas_dims() const -> std::pair<int32_t, int32_t>;
    auto atlas_id() const -> uint32_t;

    constexpr static uint8_t FIRST_GLYPH = 32;
    constexpr static uint8_t LAST_GLYPH = 127;
    constexpr static uint32_t GLYPH_COUNT = LAST_GLYPH - FIRST_GLYPH + 1;

    struct Glyph {
        emath::vec2 offset;   // world position of the bottom‑left of the glyph
        uint16_t texRect[4];  // (uMin, vMin, uMax, vMax) in atlas
    };

    static auto glyphs() -> std::array<stbtt_bakedchar, UiFont::GLYPH_COUNT>&;

private:
    const char* m_Name;
    float m_Size;
    struct stbtt_fontinfo* m_Handle;
    uint32_t m_AtlasId;
};