#pragma once

#include "LightBuffer.h"

#include "LightData.h"

#include <LibMath/Point/Cartesian.h>

namespace KaputEngine
{
	class PointLightComponent;
}

namespace KaputEngine::Rendering::Lighting
{
	struct PointBufferItem
	{
		PointLightData data;
		alignas(sizeof(float[4])) LibMath::Cartesian3f position;
	};

	class PointLightBuffer final
		: public LightBufferBase<16, sizeof(PointBufferItem)>
	{
	public:
		PointLightBuffer() = default;

	protected:
		void updateLight(const LightComponent& light) noexcept final;
	};
}
