out vec3 v_Dir;

uniform mat4 Projection;
uniform mat4 View;

vec2 a_Pos = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2) * 2.0 - 1.0;

void main() {

    gl_Position = vec4(a_Pos, 1.0, 1.0);

    vec4 ray = inverse(Projection) * vec4(a_Pos, 1.0, 1.0);

    v_Dir = (inverse(mat4(mat3(View))) * ray).xyz;
}