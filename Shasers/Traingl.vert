#version 330 core

layout(location = 0) in vec3 aPos;
uniform mat4 u_mat;
uniform mat4 persp;

void main() {
    gl_Position = persp * u_mat * vec4(aPos, 1.0);
}