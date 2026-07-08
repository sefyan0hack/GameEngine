in vec3 Normal;
in vec3 WorldPos;
in vec2 Uv;

out vec4 FragColor;

uniform sampler2D uDiffuseMap;

layout(std140) uniform Camera
{
    mat4 Projection;
    mat4 View;
    vec3 Position;
} Cam;

layout(std140) uniform SunLight
{
    vec3 Direction;
    vec3 Color;
    vec3 Ambient;
} Sun;

void main()
{    
    vec3 N = normalize(Normal);

    vec3 lightDir = normalize(-Sun.Direction);

    // diffuse
    float diff = max(dot(N, lightDir), 0.0);

    // specular
    vec3 viewDir = normalize(Cam.Position - WorldPos);
    vec3 reflectDir = reflect(-lightDir, N);

    float spec = pow(
        max(dot(viewDir, reflectDir), 0.0),
        32.0
    );

    vec3 albedo = texture(uDiffuseMap, Uv).rgb;

    vec3 ambient  = Sun.Ambient * albedo;
    vec3 diffuse  = diff * Sun.Color * albedo;
    vec3 specular = spec * Sun.Color * 0.35;

    vec3 color = ambient + diffuse + specular;

    FragColor = vec4(color, 1.0);
}