#define MAX_POINT_LIGHTS 16

struct BaseLight
{
	vec4 color;
	float intensity;
	bool enabled;
};

struct DirectionalLight
{
	BaseLight base;
	vec3 direction;
};

struct PointLight
{
	BaseLight base;
	vec3 position;
};

layout(std430, binding = 0) buffer directionalBuffer
{
	DirectionalLight directionalLight;
};

layout(std430, binding = 1) buffer pointBuffer
{
	PointLight pointLights[MAX_POINT_LIGHTS];
};
