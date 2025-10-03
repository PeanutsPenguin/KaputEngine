#include "Rendering/Lighting/DirectionalLightBuffer.h"

#include "Component/Lighting/DirectionalLightComponent.h"
#include "Rendering/Buffer/SharedBuffer.hpp"

using namespace KaputEngine;
using namespace KaputEngine::Rendering::Lighting;

void DirectionalLightBuffer::updateLight(const LightComponent& light) noexcept
{
	const DirectionalLightData& data = static_cast<const DirectionalLightComponent&>(light).getData();

	if (data.enabled)
		m_buffer.write(0, data);
	else
		// Only write disabled state, other data can be left unchanged
		m_buffer.write(offsetof(DirectionalLightData, enabled), data.enabled);
}
