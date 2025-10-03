#version 430 core

#include "../Dependency/PBR.glsl"

layout(location = 0) uniform MaterialSampler materialSampler;
uniform Camera camera;
uniform mat4 model;

in mat3 TBN;
in vec3 worldPos;
in MaterialData materialData;

out vec4 FragColor;

void main()
{
	// Pull material info from vert based on each sampler mode
	MaterialValues material = pullMaterial(materialSampler, materialData);

	// Normal vector
	vec3 n = material.normal;

	// Convert the mapped normal from color to tangent space normal and bring to world space
	if (materialSampler.normal.mode == 2)
		n = normalize(TBN * (n * 2.0 - 1.0));

	// View vector
	vec3 v = normalize(camera.position - worldPos);

	// 0.04 is an established good index for all material types. For more accuracy, use the standard Fresnel index chart based on material type (metal, water, etc.)
	vec3 f0 = vec3(0.04);

	// Apply the Fresnel index to apply colored highlights for metallic materials
	f0 = mix(f0, material.albedo.rgb, material.metallic);

	// Light energy reaching the fragment
	vec3 lo = vec3(0);

	// Accumulate light energy from the single directional light
	if (directionalLight.base.enabled)
		lo += calculateDirectionalLightLo(directionalLight, material, v, n, f0);

	// Accumulate light energy from each enabled point light
	for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
	{
		if (!pointLights[i].base.enabled)
			continue;

		lo += calculatePointLightLo(pointLights[i], material, worldPos, v, n, f0);
	}

	// Apply ambient occlusion with a light amount of the original color for visiblity in complete darkness
	vec3 ambient = vec3(0.03) * material.albedo.rgb * material.ambientOcclusion;

	// Apply accumulated light energy
	vec3 color = ambient + lo;

	// Gamma correct
	color = color / (color + vec3(1));
	color = pow(color, vec3(1 / 2.2));

	FragColor = vec4(color, 0);
}
