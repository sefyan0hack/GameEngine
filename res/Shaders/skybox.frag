out vec4 FragColor;

in vec3 v_Dir;

uniform samplerCube uDiffuseMap;

void main() {
  FragColor = texture(uDiffuseMap, v_Dir);
}