//Luminance (Grayscale conversion)
float luminance(vec3 color) {
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}
// UV Tiling and Offset
vec2 scaleOffset(vec2 uv, vec2 scale, vec2 offset) {
    return uv * scale + offset;
}
// Saturation
vec3 saturate(vec3 rgb, float adjustment) {
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(rgb, W));
    return mix(intensity, rgb, adjustment);
}
// checker board pattren
vec3 checkerboard(vec2 u)
{
    vec2 f = u - floor(u);
    return ((f.x < 0.5)) ^^ (f.y < 0.5) ? vec3(1, 0, 0) : vec3(0, 0, 1);
}

#line 1 // The #line statement is there so that compiler error messages give the correct line numbers
