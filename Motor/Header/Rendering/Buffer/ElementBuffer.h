#pragma once

#include "Buffer.h"

#include <span>

namespace KaputEngine::Rendering::Buffer
{
	class ElementBuffer final : public Buffer
	{
	public:
		ElementBuffer() = default;
		ElementBuffer(const ElementBuffer&) = delete;
		ElementBuffer(ElementBuffer&&) noexcept = default;

		~ElementBuffer() final;

		ElementBuffer& operator=(ElementBuffer&&) = default;

		void create(const std::span<const unsigned int>& indices);

		void bind() const override;
		void unbind() const override;

		_NODISCARD const int& count() const noexcept;

	private:
		int m_count = 0;
	};
}
