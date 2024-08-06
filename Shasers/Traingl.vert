#version 330 core

layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 offset;

out vec2 TexCoord;

uniform mat4 Modle;
uniform mat4 Perspective;
uniform mat4 Camera;

void main() {
    gl_Position = Perspective * Camera * Modle * vec4(aPos + offset, 1.0);
    //gl_Position = Perspective * Camera * Modle * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}