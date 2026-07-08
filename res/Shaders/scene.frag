in vec3 Normal;
in vec3 WorldPos;
in vec2 Uv;

out vec4 FragColor;

layout(std140) uniform Camera
{
    mat4 Projection;
    mat4 View;
    vec3 CameraPos;
};

uniform sampler2D uDiffuseMap;

const vec3 LightPosition = vec3(5.0, 5.0, 5.0);
const vec3 LightColor = vec3(1.0);
const vec3 AmbientColor = vec3(0.04);

void main()
{    
    vec3 N = normalize(Normal);

    vec3 lightDir = normalize(LightPosition - WorldPos);

    // diffuse
    float diff = max(dot(N, lightDir), 0.0);

    // specular
    vec3 viewDir = normalize(CameraPos - WorldPos);
    vec3 reflectDir = reflect(-lightDir, N);

    float spec = pow(
        max(dot(viewDir, reflectDir), 0.0),
        32.0
    );

    vec3 albedo = texture(uDiffuseMap, Uv).rgb;

    vec3 ambient = AmbientColor * albedo;

    vec3 diffuse = diff * LightColor * albedo;

    vec3 specular = spec * LightColor * 0.35;

    vec3 color = ambient + diffuse + specular;

    // gamma correction
    // color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}