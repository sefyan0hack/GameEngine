in vec3 Normal;
in vec3 WorldPos;
in vec2 Uv;

out vec4 FragColor;

uniform sampler2D uDiffuseMap;

const vec3 AmbientColor = vec3(0.2);
const vec3 LightDir =  normalize(vec3(0.0, -0.5, 0.0)); // normalized, points FROM light

void main()
{
    vec4 baseColor = texture(uDiffuseMap, Uv);

    vec3 N = normalize(Normal);

    float diff = max(dot(N, -LightDir), 0.0);

    vec3 lighting = AmbientColor + vec3(diff);

    FragColor = vec4(baseColor.rgb * lighting, baseColor.a);
}