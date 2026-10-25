layout(location = 0) in vec3 aPosition;
layout (location = 3) in mat4 aModel;

layout(std140) uniform Camera
{
    mat4 Projection;
    mat4 View;
    vec3 Position;
} Cam;


void main()
{
    gl_Position = Cam.Projection * Cam.View * aModel * vec4(aPosition, 1.0);
}