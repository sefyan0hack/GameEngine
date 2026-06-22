// in vec3 Normal;
in vec3 WorldPos;
in vec2 Uv;

out vec4 FragColor;

uniform sampler2D uDiffuseMap;

const vec3 AmbientColor = vec3(0.2);
const vec3 LightDir =  normalize(vec3(0.7, -0.5, 0.3)); // normalized, points FROM light

void main()
{
    vec3 dx = dFdx(WorldPos);
    vec3 dy = dFdy(WorldPos);
    vec3 normal = normalize(cross(dx, dy));
    vec3 N = normalize(normal);

    vec4 baseColor = texture(uDiffuseMap, Uv);

    float diff = max(dot(N, -LightDir), 0.0);

    vec3 lighting = AmbientColor + vec3(diff);

    FragColor = vec4(baseColor.rgb * lighting, baseColor.a);
}