
in vec3 vNormal;               // Interpolated from vertex shader
in vec2 vTexCoord;             
in vec3 vWorldPos;

uniform vec3 uCameraPos;       // For specular lighting
uniform sampler2D uDiffuseMap; // Texture sampler
uniform vec3 uLightPos;        // World‐space light position
uniform vec3 uLightColor;      // Light color/intensity
uniform vec3 uAmbientColor;    // Ambient light component

out vec4 FragColor;

void main() {
    // Normalize interpolated normal
    vec3 N = normalize(vNormal);
    
    // Compute simple diffuse lighting
    vec3 L = normalize(uLightPos - vWorldPos);
    float diff = max(dot(N, L), 0.0);
    
    // Simple ambient + diffuse
    vec3 albedo = texture(uDiffuseMap, vTexCoord).rgb;
    vec3 ambient = uAmbientColor * albedo;
    vec3 diffuse = diff * uLightColor * albedo;
    
    // Optionally add simple specular
    vec3 V = normalize(uCameraPos - vWorldPos);
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 32.0);
    vec3 specular = spec * uLightColor;
    
    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}
