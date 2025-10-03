//? #version 330 core

struct MetallicSampler
{
	int mode;
	float value;
	sampler2D map;
};

vec2 pushMetallic(MetallicSampler sampler, float metallicAttrib, vec2 uv)
{
	switch (sampler.mode)
	{
	case 0: // constant
		return vec2(sampler.value, 0);
	case 1: // attribute
		return vec2(metallicAttrib, 0);
	case 2: // texture
		return uv;
	}
}

float pullMetallic(MetallicSampler sampler, vec2 data)
{
	return sampler.mode == 2 ? texture(sampler.map, data).r : data.x;
}
