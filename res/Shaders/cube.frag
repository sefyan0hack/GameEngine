in vec3 Normal;
in vec3 WorldPos;
in vec2 Uv;

out vec4 FragColor;

uniform sampler2D albedo;
uniform samplerCube skybox;
uniform vec3 cameraPos;

void main() {
    float reflectivity = 0.2;
    float ratio = 1.00 / 1.46;

    // Sample the base texture
    vec4 baseColor = texture(albedo, Uv);

    // Calculate the view vector (from the fragment to the camera)
    vec3 viewDir = normalize(WorldPos - cameraPos);

    // Compute the reflection vector using the normalized normal
    vec3 reflVec = reflect(viewDir, normalize(Normal));

    // Compute the refraction vector using the ratio
    vec3 refrVec = refract(viewDir, normalize(Normal), ratio);

    // Compute Fresnel factor using Schlick's approximation
    float cosTheta = clamp(dot(-viewDir, normalize(Normal)), 0.0, 1.0);
    float fresnel = reflectivity + (1.0 - reflectivity) * pow(1.0 - cosTheta, 5.0);

    // Sample the environment cubemap with both vectors
    vec4 envReflectColor = texture(skybox, reflVec);
    vec4 envRefractColor = texture(skybox, refrVec);

    // Blend the reflection and refraction based on the Fresnel term
    vec4 envColor = mix(envRefractColor, envReflectColor, fresnel);

    // Blend the two colors based on the reflectivity factor
    FragColor = mix(baseColor, envColor, reflectivity);

}
