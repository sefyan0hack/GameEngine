
in vec2 v_TexCoord;

out vec4 FragColor;

uniform sampler2D u_Texture;
uniform vec3 u_Color;

void main() {
    FragColor = vec4(u_Color, texture(u_Texture, v_TexCoord).r);
}