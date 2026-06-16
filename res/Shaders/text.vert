layout(location = 0) in vec2 a_Offset;     // instance world offset
layout(location = 1) in uvec4 a_TexRect;    // instance (uMin, vMin, uMax, vMax)

out vec2 v_TexCoord;

uniform uint u_ScreenSize;

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

uint width  = (u_ScreenSize >> 0u)  & 65535u;
uint height = (u_ScreenSize >> 16u) & 65535u;

void main() {
    vec2 pos = positions[gl_VertexID];
    vec2 uv  = uvs[gl_VertexID];


    vec2 glyphSize = vec2(a_TexRect.zw) - vec2(a_TexRect.xy);

    vec2 worldPos = a_Offset + pos * glyphSize ;
    
    gl_Position = vec4(
        (worldPos.x / width) * 2.0 - 1.0,
        (worldPos.y / height) * 2.0 - 1.0,
        0.0,
        1.0
    );

    vec2 texMin = vec2(a_TexRect.xy);
    vec2 texMax = vec2(a_TexRect.zw);

    v_TexCoord = texMin + uv * (texMax - texMin);
}