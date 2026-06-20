out vec3 v_Dir;

uniform mat4 u_InvProjection;
uniform mat3 u_InvViewRot;

vec2 pos = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2) * 2.0 - 1.0;

void main() {
    gl_Position = vec4(pos, 1.0, 1.0);

    vec4 ray = u_InvProjection * vec4(pos, 1.0, 1.0);
    v_Dir = u_InvViewRot * ray.xyz;
}