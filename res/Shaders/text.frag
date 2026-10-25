
in vec2 v_TexCoord;

out vec4 FragColor;

uniform sampler2D u_Texture; // why not store also glyph info here
uniform vec3 u_Color;

void main() {
    float glyph = texture(u_Texture, v_TexCoord).r;
    FragColor = vec4(u_Color, glyph);
}