out vec4 FragColor;
in vec3 Uv;

uniform samplerCube uDiffuseMap;

void main() {
    FragColor = texture(uDiffuseMap, Uv);
}
