#pragma once

#include "Material.h"

namespace KaputEngine::Rendering
{
	template<typename T>
	inline SamplerLayer<T> MaterialLayer::getSampler(const Sampler<T>& (Material::* sampler)() const) const noexcept
	{
		return SamplerLayer<T>
		{
			.primary = (primary.*sampler)(),
			.fallback = fallback ? &(fallback->*sampler)() : nullptr
		};
	}
}
