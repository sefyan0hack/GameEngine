layout(location = 0) in vec3 aPosition;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    gl_Position = Projection * View * Model * vec4(aPosition, 1.0);
}