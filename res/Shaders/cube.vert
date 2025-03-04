#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;
layout (location = 3) in vec3 InstancePosition;

out vec3 Normal;
out vec3 FragPos;
out vec2 Uv;

uniform mat4 Modle;
uniform mat4 Perspective;
uniform mat4 View;

void main() {
    vec4 worldPos = Modle * vec4(aPos + InstancePosition, 1.0);
    Normal = mat3(transpose(inverse(Modle))) * aNormal;
    FragPos = worldPos.xyz;
    Uv = aUv;
    
    gl_Position = Perspective * View * worldPos;

}
