layout(location = 0) in vec3 aPosition;

layout(std140) uniform Camera
{
    mat4 Projection;
    mat4 View;
};

uniform mat4 Model;

void main()
{
    gl_Position = Projection * View * Model * vec4(aPosition, 1.0);
}