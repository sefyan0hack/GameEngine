#version 440 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 Perspective;
uniform mat4 View;

void main() {
    TexCoord = aPos;
    //gl_Position = Perspective * View * vec4(aPos, 1.0);
    
    vec4 pos = Perspective * View * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
