#version 440 core

out vec4 FragColor;
in vec3 TexCoord;

uniform samplerCube ourTexture;

void main() {
    FragColor = texture(ourTexture, TexCoord);
}
