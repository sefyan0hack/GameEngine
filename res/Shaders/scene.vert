layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

out vec3 Normal;
out vec3 WorldPos;
out vec2 Uv;

layout(std140) uniform Camera
{
    mat4 Projection;
    mat4 View;
    vec3 CameraPos;
};

uniform mat4 Model;

void main() {
    vec4 worldPos_ = Model * vec4(aPosition, 1.0);
    WorldPos = worldPos_.xyz;

    mat3 normalMatrix = mat3(transpose(inverse(Model)));
    Normal = normalMatrix * aNormal;

    Uv = aUv;

    gl_Position = Projection * View * worldPos_;
}