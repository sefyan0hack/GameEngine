#version 330 core

layout(location = 0) in vec3 aPos;
uniform float u_offx;
uniform float u_offy;
uniform float u_offz;
void main() {

    gl_Position = vec4(aPos.x + u_offx, aPos.y + u_offy, aPos.z + u_offz, 1.0);
    
}