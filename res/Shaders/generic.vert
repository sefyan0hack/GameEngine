
layout(location = 0) in vec3 aPos;   // Vertex position attribute
layout(location = 1) in vec3 aNormal;     // Vertex normal attribute (optional)
layout(location = 2) in vec2 aUv;   // Vertex UV attribute (optional)

uniform mat4 Model;            // Model matrix
uniform mat4 View;             // View matrix
uniform mat4 Projection;       // Projection matrix

out vec3 Normal;               // To fragment shader
out vec2 Uv;                   // To fragment shader
out vec3 WorldPos;             // To fragment shader

void main() {
    // Transform vertex position into world space
    vec4 worldPos_ = uModel * vec4(aPos, 1.0);
    WorldPos = worldPos_.xyz;
    
    // Pass through normal (if using lighting)
    mat3 normalMatrix = mat3(transpose(inverse(Model)));
    Normal = normalMatrix * aNormal;
    
    // Pass through texture coordinate
    Uv = aUv;
    
    // Compute final clip‐space position
    gl_Position = Projection * uView * worldPos;
}
