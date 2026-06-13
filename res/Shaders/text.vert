layout(location = 0) in vec2 a_Offset;     // instance world offset
layout(location = 1) in uvec4 a_TexRect;    // instance (uMin, vMin, uMax, vMax)

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

    vec2 glyphSize  = a_TexRect.zw - a_TexRect.xy;

    vec2 worldPos = a_Offset + pos * glyphSize ;
    
    gl_Position = u_Projection * vec4(worldPos, 0.0, 1.0);

    v_TexCoord = mix(a_TexRect.xy, a_TexRect.zw, uv);
}