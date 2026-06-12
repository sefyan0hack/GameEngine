layout(location = 0) in vec2 a_Offset;     // instance world offset
layout(location = 1) in vec2 a_Size;       // instance size
layout(location = 2) in vec4 a_TexRect;    // instance (uMin, vMin, uMax, vMax)

out vec2 v_TexCoord;

uniform mat4 u_Projection;

const vec2 positions[4] = vec2[](
    vec2(0.0, 0.0), // bottom-left
    vec2(1.0, 0.0), // bottom-right
    vec2(1.0, 1.0), // top-right
    vec2(0.0, 1.0)  // top-left
);

const vec2 uvs[4] = vec2[](
    vec2(0.0, 1.0), // bottom-left
    vec2(1.0, 1.0), // bottom-right
    vec2(1.0, 0.0), // top-right
    vec2(0.0, 0.0)  // top-left
);

void main() {
    vec2 pos = positions[gl_VertexID];
    vec2 uv  = uvs[gl_VertexID];

    // Scale the local quad to the glyph size, then translate
    vec2 worldPos = a_Offset + pos * a_Size;
    gl_Position = u_Projection * vec4(worldPos, 0.0, 1.0);

    // Remap local tex coord (0..1) to the glyph’s rectangle in the atlas
    v_TexCoord = vec2(mix(a_TexRect.x, a_TexRect.z, uv.x), mix(a_TexRect.y, a_TexRect.w, uv.y));
}