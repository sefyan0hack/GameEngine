layout (location = 0) in vec3 aPos;

out vec3 Uv;

uniform mat4 Perspective;
uniform mat4 View;

void main() {
    Uv = aPos;
    vec4 pos = Perspective * View * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
