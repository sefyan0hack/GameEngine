#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 u_mat;
out vec3 ou_color;
void main() {
    ou_color = aColor;
    gl_Position = u_mat * vec4(aPos, 1.0);
}