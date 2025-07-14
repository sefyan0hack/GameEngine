
in vec3 Normal;               // Interpolated from vertex shader
in vec2 Uv;
in vec3 WorldPos;

uniform vec3 uCameraPos;       // For specular lighting
uniform sampler2D uDiffuseMap; // Texture sampler
uniform vec3 uLightPos = vec3(10, 10, 10); // World‐space light position
uniform vec3 uLightColor = vec3(1, 0, 0);      // Light color/intensity
uniform vec3 uAmbientColor = vec3(1, 1, 1);    // Ambient light component
uniform samplerCube uSkyboxMap;

out vec4 FragColor;

void main() {
    texture(uSkyboxMap, vec3(1.0));// dummy

    // Normalize interpolated normal
    vec3 N = normalize(Normal);
    
    // Compute simple diffuse lighting
    vec3 L = normalize(uLightPos - WorldPos);
    float diff = max(dot(N, L), 0.0);
    
    // Simple ambient + diffuse
    vec3 albedo = texture(uDiffuseMap, Uv).rgb;
    vec3 ambient = uAmbientColor * albedo;
    vec3 diffuse = diff * uLightColor * albedo;
    
    // Optionally add simple specular
    vec3 V = normalize(uCameraPos - WorldPos);
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 32.0);
    vec3 specular = spec * uLightColor;
    
    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}
