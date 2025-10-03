#pragma once

#include "ShaderProgram.h"

#include "Sampler.h"
#include "TextureSample.h"

namespace KaputEngine::Rendering
{
	template <typename T>
	void ShaderProgram::setUniform(const UniformReference& uniform, const T& value) const
	{
		setUniform(uniform, &value, 1);
	}

	template <typename T>
	void ShaderProgram::setUniform(
		const UniformReference& uniform, _In_reads_(count) const Sampler<T>* const values, const int count) const
	{
		int location = getLocation(uniform);

		if (location == -1)
		{
			investigateUniform(uniform, ".mode");
			return;
		}

		for (int i = 0; i < count; ++i)
		{
			const Sampler<T>& sampler = values[i];

			// Texture not defined or not available - Use fallback
			if (!sampler.texture().texture || !sampler.texture().texture->id())
			{
				// Assign fallback mode, offset by 1 to ignore layer mode
				setUniform(location, static_cast<int>(sampler.fallbackMode()) - 1);

				if (sampler.fallbackMode() == eSamplerFallback::GLOBAL)
					setUniform(location + 1, sampler.global());
			}
			else
			{
				// Set mode to texture
				setUniform(location, 2);

				const TextureSample& texSample = sampler.texture();

				texSample.texture->activate(texSample.unitIndex);

				// Assign texture unit to sampler2D - Texture unit requires signed int
				setUniform(location + 2, (int)texSample.unitIndex);
			}

			location += 3;
		}
	}

	template<typename T>
	void ShaderProgram::setUniform(
		const UniformReference& uniform, _In_reads_(count) const SamplerLayer<T>* values, int count) const
	{
		int location = getLocation(uniform);

		if (location == -1)
		{
			investigateUniform(uniform, ".mode");
			return;
		}

		for (int i = 0; i < count; ++i)
		{
			const SamplerLayer<T>& layer = values[i];
			const std::shared_ptr<const Buffer::TextureBuffer> tex = layer.primary.texture().texture;

			if ((tex && tex->id()) || layer.primary.fallbackMode() != eSamplerFallback::LAYER)
				setUniform(location, layer.primary);
			else if (layer.fallback)
				setUniform(location, *layer.fallback);

			location += 3;
		}
	}
}
