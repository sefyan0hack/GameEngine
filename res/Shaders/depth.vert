layout(location = 0) in vec3 aPosition;

layout(std140) uniform Camera
{
    mat4 Projection;
    mat4 View;
    vec3 Position;
} Cam;

uniform mat4 Model;

void main()
{
    gl_Position = Cam.Projection * Cam.View * Model * vec4(aPosition, 1.0);
}