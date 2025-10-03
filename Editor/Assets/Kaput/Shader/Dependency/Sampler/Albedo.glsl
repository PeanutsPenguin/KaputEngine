//? #version 330 core

struct AlbedoSampler
{
	int mode;
	vec4 value;
	sampler2D map;
};

vec4 pushAlbedo(AlbedoSampler sampler, vec4 albedoAttrib, vec2 uv)
{
	switch (sampler.mode)
	{
	case 0: // constant
		return sampler.value;
	case 1: // attribute
		return albedoAttrib;
	case 2: // texture
		return vec4(uv, 0, 0);
	}
}

vec4 pullAlbedo(AlbedoSampler sampler, vec4 data)
{
	return sampler.mode == 2 ? texture(sampler.map, data.xy) : data;
}
