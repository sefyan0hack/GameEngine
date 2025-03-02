#version 440 core

out vec4 FragColor;
in vec3 TexCoord;

uniform samplerCube albedo;

void main() {
    FragColor = texture(albedo, TexCoord);
}
