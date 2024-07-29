#version 330 core

out vec4 FragColor;
in vec3 ou_color;
void main() {
    FragColor = vec4(ou_color, 1.0);
}