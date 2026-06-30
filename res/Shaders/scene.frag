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
    vec3 normal = cross(dx, dy);

    vec3 norm = normalize(normal);
    
    vec3 lightDirTowards = normalize(-LightDir);
    
    float diff = max(dot(norm, lightDirTowards), 0.0);
    
    vec3 baseColor = texture(uDiffuseMap, Uv).rgb;
    
    vec3 diffuseLight = diff * vec3(1.0); 
    
    vec3 finalColor = (AmbientColor + diffuseLight) * baseColor;

    FragColor = vec4(finalColor, 1.0);
}