#pragma once

#include "Rendering/Buffer/SharedBuffer.h"

#include <bitset>

namespace KaputEngine
{
	class LightComponent;
}

namespace KaputEngine::Rendering::Lighting
{
	class LightBuffer
	{
		friend KaputEngine::LightComponent;

	public:
		LightBuffer() = default;
		LightBuffer(const LightBuffer&) = delete;
		LightBuffer(LightBuffer&&) = delete;

		_NODISCARD const Buffer::SharedBuffer& buffer() const noexcept;

		void destroy();

	protected:
		virtual void updateLight(const LightComponent& light) noexcept  = 0;

		_NODISCARD virtual size_t registerLight(const LightComponent& light) = 0;
		virtual void unregisterLight(const LightComponent& light) = 0;

		Buffer::SharedBuffer m_buffer;
	};

	template <size_t _MaxLights, size_t _Stride>
	class LightBufferBase : public LightBuffer
	{
	public:
		static constexpr size_t MaxLights = _MaxLights;

		_NODISCARD constexpr size_t getCount() const noexcept;

		void create(unsigned int index);

	protected:
		LightBufferBase() = default;

		_NODISCARD size_t registerLight(const LightComponent& light) final;
		void unregisterLight(const LightComponent& light) final;

		static constexpr size_t Stride = _Stride;
		size_t m_count;

		std::bitset<_MaxLights> m_bufferSlots;
	};
}
