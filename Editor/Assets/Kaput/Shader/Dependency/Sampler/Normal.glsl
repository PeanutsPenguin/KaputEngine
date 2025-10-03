//? #version 330 core

struct NormalSampler
{
	int mode;
	vec3 value;
	sampler2D map;
};

vec3 pushNormal(NormalSampler sampler, vec3 normalAttrib, vec2 uv)
{
	switch (sampler.mode)
	{
	case 0: // constant
		return sampler.value;
	case 1: // attribute
		return normalAttrib;
	case 2: // texture
		return vec3(uv, 0);
	}
}

vec3 pullNormal(NormalSampler sampler, vec3 data)
{
	return sampler.mode == 2 ? texture(sampler.map, data.xy).rgb : data;
}

mat3 getTBN(mat4 model, vec3 normal, vec3 tangent, vec3 bitangent)
{
	vec3 t = normalize(vec3(model * vec4(tangent,   0.0)));
	vec3 b = normalize(vec3(model * vec4(bitangent, 0.0)));
	vec3 n = normalize(vec3(model * vec4(normal,    0.0)));

	return mat3(t, b, n);
}
