#version 430 core

#include "../Dependency/Vertex.glsl"
#include "../Dependency/Camera.glsl"
#include "../Dependency/Sampler/Material.glsl"

layout(location = 0) uniform MaterialSampler materialSampler;
uniform mat4 model;
uniform Camera camera;

out vec3 worldPos;
out MaterialData materialData;
out mat3 TBN;

void main()
{
	MaterialValues materialAttributes;
	materialAttributes.albedo = aAlbedo;
	materialAttributes.normal = aNormal;

	// Push material info to frag based on each sampler mode
	materialData = pushMaterial(model, materialSampler, materialAttributes, aTexCoords);

	// Get tangent to world space matrix for normal mapping
	if (materialSampler.normal.mode == 2)
		TBN = getTBN(model, aNormal, aTangent, aBitangent);

	worldPos = (model * vec4(aPos.xyz, 1.0)).xyz;
	gl_Position = cameraTransformWorld(camera, worldPos);
}
