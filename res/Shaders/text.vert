layout(location = 0) in vec2  a_Position; // instance world offset
layout(location = 1) in uvec4 a_UvRect;   // instance (uMin, vMin, uMax, vMax)

out vec2 v_TexCoord;

layout(std140) uniform Globle
{
    uvec2 ScreenResolution;
    uint Frame;
    uint Flags;
};

uniform sampler2D u_Texture;

void main() {

    vec2 corner = vec2(
        float(gl_VertexID >> 1),
        float(gl_VertexID & 1)
    );

    vec2 texMin = vec2(a_UvRect.xy);
    vec2 glyphSize = vec2(a_UvRect.zw - a_UvRect.xy);

    // Screen-space position in pixels
    vec2 worldPos = a_Position + corner * glyphSize;

    const float scale = 1.0;

    // Clip-space
    vec2 clipPos = worldPos * scale * (vec2(2.0) / vec2(ScreenResolution)) - 1.0;
    clipPos.y = -clipPos.y;

    gl_Position = vec4(clipPos, 0.0, 1.0);

    vec2 InvAtlasSize = 1.0 / vec2(textureSize(u_Texture, 0));

    v_TexCoord = (texMin + corner * glyphSize) * InvAtlasSize;
}