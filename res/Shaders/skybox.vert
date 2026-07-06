out vec3 v_Dir;

layout(std140) uniform Camera
{
    mat4 Projection;
    mat4 View;
};

void main() {
    mat4 InvProjection = inverse(Projection);
    mat3 InvViewRot = mat3(transpose(View));

    vec2 pos = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2) * 2.0 - 1.0;

    gl_Position = vec4(pos, 1.0, 1.0);

    vec4 ray = InvProjection * vec4(pos, 1.0, 1.0);
    v_Dir = InvViewRot * ray.xyz;
}