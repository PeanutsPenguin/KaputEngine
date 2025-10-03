#pragma once

#include "Rendering/Color.h"
#include "Rendering/GlTypes.h"

#include <LibMath/Vector/Vector3.h>

namespace KaputEngine::Rendering::Lighting
{
	struct LightData
	{
		alignas(sizeof(float[4])) Color color;
		alignas(sizeof(float)) float intensity = 1.f;
		alignas(sizeof(RenderBool)) RenderBool enabled = false;

		_NODISCARD bool operator==(const LightData& other) const noexcept = default;
		_NODISCARD bool operator!=(const LightData& other) const noexcept = default;
	};

	struct DirectionalLightData : LightData
	{
		alignas(sizeof(float[4])) LibMath::Vector3f direction = LibMath::Vector3f::Dir::one();

		_NODISCARD bool operator==(const DirectionalLightData& other) const noexcept = default;
		_NODISCARD bool operator!=(const DirectionalLightData& other) const noexcept = default;
	};

	struct PointLightData : LightData
	{
		// Using distinct struct for safety
		// Data actually contains the world position, fetched at runtime from the component rather than stored here

		_NODISCARD bool operator==(const PointLightData& other) const noexcept = default;
		_NODISCARD bool operator!=(const PointLightData& other) const noexcept = default;
	};
}
