//? #version 330 core

struct RoughnessSampler
{
    int mode;
    float value;
    sampler2D map;
};

vec2 pushRoughness(RoughnessSampler sampler, float roughnessAttrib, vec2 uv)
{
    switch (sampler.mode)
    {
    case 0: // constant
        return vec2(sampler.value, 0);
    case 1: // attribute
        return vec2(roughnessAttrib, 0);
    case 2: // texture
        return uv;
    }
}

float pullRoughness(RoughnessSampler sampler, vec2 data)
{
    return sampler.mode == 2 ? texture(sampler.map, data).r : data.x;
}
