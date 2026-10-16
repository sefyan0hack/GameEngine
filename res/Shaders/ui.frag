
layout (std140) uniform Globle
{
    uvec2 ScreenResolution;
    uint Frame;
    uint Flags;
};

uniform int   uShape;
uniform vec4  uPosition;
uniform vec4  uColor;
uniform vec4  uParams;


out vec4 FragColor;

float rounded_rectangle(vec2 pos, vec2 size, float radius)
{
    vec2 q = abs(pos) - size * 0.5 + radius;

    return length(max(q, 0.0))
         + min(max(q.x, q.y), 0.0)
         - radius;
}

float rectangle(vec2 pos, vec2 size)
{
    return max(abs(pos.x) - size.x * 0.5, abs(pos.y) - size.y * 0.5);
}

float circle(vec2 pos, float radius)
{
    return length(pos) - radius;
}

float circle_outline(vec2 pos, float radius, float thickness)
{
    return abs(circle(pos, radius)) - thickness * 0.5;
}

float line(vec2 p, vec2 size, float thickness)
{
    vec2 a = vec2(-size.x * 0.5, -size.y * 0.5);
    vec2 b = vec2( size.x * 0.5,  size.y * 0.5);

    vec2 pa = p - a;
    vec2 ba = b - a;

    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);

    return length(pa - ba * h) - thickness * 0.5;
}

float rectangle_outline(vec2 pos, vec2 size, float thickness)
{
    return abs(rectangle(pos, size)) - thickness * 0.5;
}

float rounded_rectangle_outline(vec2 pos, vec2 size, float radius, float thickness)
{
    return abs(rounded_rectangle(pos, size, radius)) - thickness * 0.5;
}

float shapeSDF()
{
    // gl_FragCoord origin is bottom-left; UI origin is top-left.
    vec2 pixel = vec2(
        gl_FragCoord.x,
        float(ScreenResolution.y) - gl_FragCoord.y
    );

    // Position relative to the shape's center.
    vec2 size = uPosition.zw;
    vec2 center = uPosition.xy + size * 0.5;
    vec2 p = pixel - center;

    switch (uShape)
    {
        case GLSL_SHAPE_rectangle: return rectangle(p, size);
        case GLSL_SHAPE_rounded_rectangle: return rounded_rectangle(p, size, uParams.x);
        case GLSL_SHAPE_circle: return circle(p, uParams.x);
        case GLSL_SHAPE_line: return line(p, size, uParams.x);
        case GLSL_SHAPE_circle_outline: return circle_outline(p, uParams.x, uParams.y);
        case GLSL_SHAPE_rectangle_outline: return rectangle_outline(p, size, uParams.x);
        case GLSL_SHAPE_rounded_rectangle_outline: return rounded_rectangle_outline(p, size, uParams.x, uParams.y);

        default: return 1e20;
    }
}

void main()
{
    float d = shapeSDF();
    float aa = max(fwidth(d), 1e-5);
    float alpha = max(1.0 - smoothstep(-aa, aa, d), 0);

    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}