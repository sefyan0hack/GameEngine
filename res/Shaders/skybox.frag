#ifdef GL_ES
    #version 300 es
    precision mediump float;
#else
    #version 440 core
#endif

out vec4 FragColor;
in vec3 Uv;

uniform samplerCube albedo;

void main() {
    FragColor = texture(albedo, Uv);
}
