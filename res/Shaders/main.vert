layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;
layout (location = 3) in vec3 InstancePosition;

out vec3 Normal;
out vec3 WorldPos;
out vec2 Uv;
out vec3 Barycentric;

uniform mat4 Model;
uniform mat4 Projection;
uniform mat4 View;

void main() {
    vec4 worldPos_ = Model * vec4(aPos + InstancePosition, 1.0);
    WorldPos = worldPos_.xyz;

    mat3 normalMatrix = mat3(transpose(inverse(Model)));
    Normal = normalMatrix * aNormal;

    Uv = aUv;
    Barycentric = vec3(0.0);
    int index = gl_VertexID % 3;

    if (index == 0) {
        Barycentric.x = 1.0;
    } else if (index == 1) {
        Barycentric.y = 1.0;
    } else {
        Barycentric.z = 1.0;
    }

    gl_Position = Projection * View * worldPos_;
}