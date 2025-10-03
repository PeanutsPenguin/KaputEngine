//? #version 430 core

#include "./Light.glsl"
#include "./Sampler/Material.glsl"
#include "./Camera.glsl"

#define PI 3.1415926535897932384626433832795

vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
    return (1 - f0) * pow(clamp(1 - cosTheta, 0, 1), 5) + f0;
}

float DistributionGGX(vec3 n, vec3 h, float roughness)
{
    float a      = roughness * roughness;
    // Use rough^4 as recommended by Disney and Epic
    float a2     = a * a;
    float nhDot  = max(dot(n, h), 0);
    float nhDot2 = nhDot * nhDot;

    float num   = a2;
    float denom = (nhDot2 * (a2 - 1) + 1);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float nvDot, float roughness)
{
    float r = roughness + 1;
    float k = r * r / 8;

    float num   = nvDot;
    float denom = nvDot * (1 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 n, vec3 v, vec3 l, float roughness)
{
    float nvDot = max(dot(n, v), 0);
    float nlDot = max(dot(n, l), 0);

    return
        GeometrySchlickGGX(nvDot, roughness) *
        GeometrySchlickGGX(nlDot, roughness);
}

vec3 calculateBaseLightLo(BaseLight light, MaterialValues material, float attenuation, vec3 l, vec3 v, vec3 n, vec3 f0)
{
    vec3 h = normalize(v + l);
    vec3 radiance = light.color.xyz * attenuation * light.intensity;

    float ndf = DistributionGGX(n, h, material.roughness);
    float g   = GeometrySmith(n, v, l, material.roughness);

    // kd/ks based on conservation of energy between reflected reflected and diffused light. kd + ks = 1
    vec3 fks    = fresnelSchlick(max(dot(h, v), 0), f0);
    vec3 kd = vec3(1) - fks;
    kd *= 1 - material.metallic;

    vec3 numerator    = ndf * g * fks;
    float denominator = 4 * max(dot(n, v), 0) * max(dot(n, l), 0) + 0.0001;
    vec3 specular     = numerator / denominator;

    float nlDot = max(dot(n, l), 0);
    return (kd * material.albedo.rgb / PI + specular) * radiance * nlDot;
}

vec3 calculateDirectionalLightLo(DirectionalLight light, MaterialValues material, vec3 v, vec3 n, vec3 f0)
{
    // Predetermined direction vector
    // Infinite range - No distance attenuation
    return calculateBaseLightLo(light.base, material, 1.0, light.direction, v, n, f0);
}

vec3 calculatePointLightLo(PointLight light, MaterialValues material, vec3 objPos, vec3 v, vec3 n, vec3 f0)
{
    // Dynamic direction vector based on the position of the light and object
    vec3 deltaPos     = light.position - objPos;
    vec3 l            = normalize(deltaPos);
    // Finite range - Attenuate based on distance
    float distance    = length(deltaPos);
    float attenuation = 1 / (distance * distance);

    return calculateBaseLightLo(light.base, material, attenuation, l, v, n, f0);
}
