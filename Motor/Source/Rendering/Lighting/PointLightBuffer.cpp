#include "Rendering/Lighting/PointLightBuffer.h"

#include "Component/Lighting/PointLightComponent.h"
#include "Rendering/Buffer/SharedBuffer.hpp"

using KaputEngine::LightComponent;
using KaputEngine::Rendering::Lighting::PointLightBuffer;

void PointLightBuffer::updateLight(const LightComponent& light) noexcept
{
	const PointLightComponent& pointLight = static_cast<const PointLightComponent&>(light);
	const PointLightData& data = pointLight.getData();

	const size_t offset = (light.index() * Stride);

	if (!data.enabled)
	{
		// Only write disabled state, other data can be left unchanged
		m_buffer.write(offset + offsetof(PointLightData, enabled), false);
		return;
	}

	PointBufferItem item
	{
		.data = pointLight.getData(),
		.position = pointLight.getWorldTransform().position
	};

	m_buffer.write(offset, item);
}
