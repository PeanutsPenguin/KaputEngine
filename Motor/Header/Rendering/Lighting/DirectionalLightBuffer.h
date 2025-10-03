#pragma once

#include "LightBuffer.h"

#include "LightData.h"

namespace KaputEngine
{
	class DirectionalLightComponent;
}

namespace KaputEngine::Rendering::Lighting
{
	class DirectionalLightBuffer final
		: public LightBufferBase<1, sizeof(DirectionalLightData)>
	{
	public:
		DirectionalLightBuffer() = default;

	protected:
		void updateLight(const LightComponent& light) noexcept final;
	};
}
