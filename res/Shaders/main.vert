layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;
layout (location = 3) in vec3 InstancePosition;

out vec3 Normal;
out vec3 WorldPos;
out vec2 Uv;

uniform mat4 Model;
uniform mat4 Projection;
uniform mat4 View;

void main() {
    vec4 worldPos_ = Model * vec4(aPos + InstancePosition, 1.0);
    WorldPos = worldPos_.xyz;

    mat3 normalMatrix = mat3(transpose(inverse(Model)));
    Normal = normalMatrix * aNormal;

    Uv = aUv;

    gl_Position = Projection * View * worldPos_;
}