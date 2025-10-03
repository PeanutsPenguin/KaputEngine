#include "./Albedo.glsl"
#include "./Normal.glsl"
#include "./Metallic.glsl"
#include "./Roughness.glsl"
#include "./AmbientOcclusion.glsl"

struct MaterialSampler
{
	AlbedoSampler albedo;
	NormalSampler normal;
	MetallicSampler metallic;
	RoughnessSampler roughness;
	AmbientOcclusionSampler ambientOcclusion;
};

struct MaterialData
{
	vec4 albedo;
	vec3 normal;
	vec2 metallic;
	vec2 roughness;
	vec2 ambientOcclusion;
};

struct MaterialValues
{
	vec4 albedo;
	vec3 normal;
	float metallic;
	float roughness;
	float ambientOcclusion;
};

MaterialData pushMaterial(mat4 model, MaterialSampler sampler, MaterialValues attributes, vec2 uv)
{
	MaterialData data;
	data.albedo           = pushAlbedo(sampler.albedo, attributes.albedo, uv);
	data.normal           = pushNormal(sampler.normal, attributes.normal, uv);
	data.metallic         = pushMetallic(sampler.metallic, attributes.metallic, uv);
	data.roughness        = pushRoughness(sampler.roughness, attributes.roughness, uv);
	data.ambientOcclusion = pushAmbientOcclusion(sampler.ambientOcclusion, attributes.ambientOcclusion, uv);
	return data;
}

MaterialValues pullMaterial(MaterialSampler sampler, MaterialData data)
{
	MaterialValues values;
	values.albedo           = pullAlbedo(sampler.albedo, data.albedo);
	values.normal           = pullNormal(sampler.normal, data.normal);
	values.metallic         = pullMetallic(sampler.metallic, data.metallic);
	values.roughness        = pullRoughness(sampler.roughness, data.roughness);
	values.ambientOcclusion = pullAmbientOcclusion(sampler.ambientOcclusion, data.ambientOcclusion);
	return values;
}
