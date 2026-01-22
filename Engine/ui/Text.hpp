#pragma once

#include <string>
#include <unordered_map>
#include <engine_export.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <glm/vec4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <vector>
#include <unordered_map>

struct AtlasGlyph {
    glm::vec2 texCoordsMin;      // Top-left texture coordinates in atlas
    glm::vec2 texCoordsMax;      // Bottom-right texture coordinates in atlas
    glm::ivec2 size;            // Size of glyph in pixels
    glm::ivec2 bearing;         // Offset from baseline to left/top of glyph
    uint32_t advance;           // Offset to advance to next glyph (in 1/64 pixels)
};

class ENGINE_EXPORT Text {
public:
    Text(const class OpenGL& ctx);
    ~Text();
    
    auto render() -> void;
    auto text(std::string text, glm::vec2 pos) -> void;
    auto print(std::string text) -> void;
    

public:
    constexpr static const char* FONT_NAME = "res/JetBrainsMonoNL-BoldItalic.ttf";
    constexpr static glm::vec3 FONT_COLOR = {0.1f, 0.85f, 0.1f};
    constexpr static uint32_t FONT_SIZE = 24;

    constexpr static uint32_t GLYPH_PADDING = 1;

    constexpr static uint8_t FIRST_GLYPH = 32;
    constexpr static uint8_t LAST_GLYPH = 127;

    constexpr static uint32_t NUM_GLYPHS = LAST_GLYPH - FIRST_GLYPH + 1;
    constexpr static uint32_t CELL_SIZE = FONT_SIZE + (GLYPH_PADDING * 2);

    constexpr static uint32_t ATLAS_COLS = std::bit_ceil(static_cast<uint32_t>(std::ceil(std::sqrt(NUM_GLYPHS))));
    constexpr static uint32_t ATLAS_ROWS = (NUM_GLYPHS + ATLAS_COLS - 1) / ATLAS_COLS;

    constexpr static uint32_t ATLAS_WIDTH  = std::bit_ceil(ATLAS_COLS * CELL_SIZE);
    constexpr static uint32_t ATLAS_HEIGHT = std::bit_ceil(ATLAS_ROWS * CELL_SIZE);

    constexpr static uint32_t MAX_QUADS = 10000;   // Max quads per batch
    constexpr static uint32_t VERTICES_PER_QUAD = 4;
    constexpr static uint32_t INDICES_PER_QUAD = 6;

private:
    struct Vertex {
        glm::vec2 position;
        glm::vec2 texCoord;
    };
    
    auto create_atlas() -> void;
    auto init_buffers() -> void;
    auto flush_batch() -> void;

    auto push_quad(const glm::vec2& position, const glm::vec2& size, 
                   const glm::vec2& texMin, const glm::vec2& texMax) -> void;
    
private:
    const class OpenGL& m_GApi;
    std::shared_ptr<class Shader> m_Vert, m_Frag;
    std::shared_ptr<class ShaderProgram> m_Program;
    
    // Buffers
    uint32_t VAO, VBO, EBO;
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
    uint32_t m_IndexCount = 0;

    // Rendering state
    uint32_t m_AtlasTexture;

    // Glyph storage
    std::unordered_map<uint32_t, AtlasGlyph> m_Glyphs;  // char32_t to glyph

    // Font metrics
    int32_t m_Ascent = 0;
    int32_t m_Descent = 0;
    int32_t m_LineGap = 0;

    // Batch rendering
    std::unordered_map<glm::vec2, std::string> m_Batches;
};