
layout (location = 0) in vec4 aPos;

out vec2 TexCoord;

uniform mat4 Projection;

void main()
{
    TexCoord = aPos.zw;
    gl_Position = Projection * vec4(aPos.xy, 0.0, 1.0);
}