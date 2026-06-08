in vec3 Normal;
in vec3 WorldPos;
in vec2 Uv;

out vec4 FragColor;

uniform vec3 Eye;
uniform sampler2D uDiffuseMap;
uniform samplerCube uSkyboxMap;

void main() {
    float reflectivity = 0.4;
    float ratio = 1.00 / 1.46;

    vec4 baseColor = texture(uDiffuseMap, Uv);

    vec3 viewDir = normalize(WorldPos - Eye);

    vec3 reflVec = reflect(viewDir, normalize(Normal));
    vec4 envReflectColor = texture(uSkyboxMap, reflVec);

    vec3 refrVec = refract(viewDir, normalize(Normal), ratio);
    vec4 envRefractColor = texture(uSkyboxMap, refrVec);

    float cosTheta = clamp(dot(-viewDir, normalize(Normal)), 0.0, 1.0);

    float fresnel = reflectivity + (1.0 - reflectivity) * pow(1.0 - cosTheta, 5.0);

    vec4 envColor = mix(envRefractColor, envReflectColor, fresnel);

    FragColor = mix(baseColor, envColor, reflectivity);
}