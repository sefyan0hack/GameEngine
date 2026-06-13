
in vec2 v_TexCoord;

out vec4 FragColor;

uniform sampler2D u_Texture;
uniform vec3 u_Color;

void main() {
    vec2 atlasSize = vec2(textureSize(u_Texture, 0));
    FragColor = vec4(u_Color, texture(u_Texture, v_TexCoord / atlasSize).r);
}