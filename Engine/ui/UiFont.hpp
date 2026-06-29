#pragma once
#include <cstdint>

class UiFont {
public:
    UiFont(const char* name);
    ~UiFont();

    struct bakedchar {
        unsigned short x0,y0,x1,y1;
        float xoff,yoff,xadvance;
    };

    auto name() const -> const char*;
    auto ascent() const -> int32_t;
    auto descent() const -> int32_t;
    auto linegap() const -> int32_t;
    auto pixel_scale(float scale) const -> float;
    auto get_code_point_bitmap_box(int codepoint, float scale_x, float scale_y, int *ix0, int *iy0, int *ix1, int *iy1) const -> void;
    auto bake_font_bitmap(int offset,
        float pixel_height, unsigned char *pixels, int pw, int ph,
        int first_char, int num_chars, bakedchar *chardata
    ) -> int;

private:
    const char* m_Name;
    struct stbtt_fontinfo* m_Handle;
};