
layout (std140) uniform Globle
{
    uvec2 ScreenResolution;
    uint Frame;
    uint Flags;
};

uniform int   uShape;
uniform vec4  uPosition;
uniform vec4  uColor;
uniform float uRadius;

out vec4 FragColor;

float roundedBoxSDF(vec2 pos, vec2 size, float radius)
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

float circle(vec2 pos, vec2 size)
{
    return length(pos) - min(size.x, size.y) * 0.5;
}

float circleOutline(vec2 pos, vec2 size, float thickness)
{
    return abs(circle(pos, size)) - thickness * 0.5;
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

float rectangleOutline(vec2 pos, vec2 size, float thickness)
{
    return abs(rectangle(pos, size)) - thickness * 0.5;
}

float roundedRectangleOutline(vec2 pos, vec2 size, float radius, float thickness)
{
    return abs(roundedBoxSDF(pos, size, radius)) - thickness * 0.5;
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
        case 0: return rectangle(p, size);
        case 1: return roundedBoxSDF(p, size, uRadius);
        case 2: return circle(p, size);
        case 3: return line(p, size, uRadius);
        case 4: return circleOutline(p, size, uRadius);
        case 5: return rectangleOutline(p, size, uRadius);
        // case 6: return roundedRectangleOutline(p, size, uRadius, uBorderThickness);

        default: return 1e20;
    }
}

void main()
{
    float d = shapeSDF();

    float aa = fwidth(d);
    float alpha = max(1.0 - smoothstep(-aa, aa, d), 0);

    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}