
in vec2 v_TexCoord;

out vec4 FragColor;

uniform sampler2D u_Texture;
uniform uint u_Color;

vec3 unpackColor(uint c)
{
    float r = float((c >> 0)  & 255u) / 255.0;
    float g = float((c >> 8)  & 255u) / 255.0;
    float b = float((c >> 16) & 255u) / 255.0;

    return vec3(r, g, b);
}

void main() {
    vec2 atlasSize = vec2(textureSize(u_Texture, 0));
    FragColor = vec4(unpackColor(u_Color), texture(u_Texture, v_TexCoord / atlasSize).r);
}