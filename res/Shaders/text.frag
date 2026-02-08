
in vec2 v_TexCoord;

out vec4 FragColor;

uniform sampler2D u_Texture;
uniform vec3 u_Color;

void main() {
    // Sample from texture atlas
    vec4 texColor = texture(u_Texture, v_TexCoord);
    
    // Use red channel as alpha (font texture is single-channel)
    float alpha = texColor.r;
    
    // Combine u_Color (RGB) with the calculated alpha (A)
    FragColor = vec4(u_Color, alpha);
}