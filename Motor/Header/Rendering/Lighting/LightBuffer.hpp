#pragma once

#include "LightBuffer.h"

#include "Component/Lighting/LightComponent.h"
#include "LightData.h"
#include "Rendering/Buffer/SharedBuffer.hpp"

#define TEMPLATE template <size_t _MaxLights, size_t _Stride>
#define LIGHTBUFFERBASE LightBufferBase<_MaxLights, _Stride>

namespace KaputEngine::Rendering::Lighting
{
	TEMPLATE void LIGHTBUFFERBASE::create(const unsigned int index)
	{
		static constexpr size_t size = _MaxLights * _Stride;

		m_buffer.create(size, index);

		uint8_t* bytes = new uint8_t[size] { };
		m_buffer.write(0, size, bytes);
		delete[] bytes;
	}

	TEMPLATE size_t LIGHTBUFFERBASE::registerLight(const LightComponent& light)
	{
		if (light.index() > -1)
		{
			std::cerr << __FUNCTION__"LightComponent is already registered as index" << light.index() << ".\n";
			return -1;
		}

		size_t lightIndex = -1;

		for (size_t i = 0; i < m_bufferSlots.size(); ++i)
			if (!m_bufferSlots[i])
			{
				lightIndex = i;
				break;
			}

		if (lightIndex == -1)
		{
			std::cerr << __FUNCTION__"Light buffer full.\n";
			return -1;
		}

		m_bufferSlots[lightIndex] = true;

		this->m_count++;

		return lightIndex;
	}

	TEMPLATE void LIGHTBUFFERBASE::unregisterLight(const LightComponent& light)
	{
		const size_t index = light.index();

		if (index)
		{
			std::cerr << __FUNCTION__"LightComponent is not registered.\n";
			return;
		}

		m_bufferSlots[index] = false;
		this->m_count--;
	}

	TEMPLATE constexpr size_t LIGHTBUFFERBASE::getCount() const noexcept
	{
		return this->m_count;
	}
}

#undef TEMPLATE
#undef LIGHTBUFFERBASE
