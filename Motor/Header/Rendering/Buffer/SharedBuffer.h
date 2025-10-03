#pragma once

#include "Buffer.h"

#include <sal.h>

namespace KaputEngine::Rendering::Buffer
{
	class SharedBuffer : public Buffer
	{
	public:
		SharedBuffer() = default;
		SharedBuffer(const SharedBuffer&) = delete;
		SharedBuffer(SharedBuffer&&) noexcept = default;

		~SharedBuffer() final;

		SharedBuffer& operator=(SharedBuffer&&) = default;

		void create(size_t size, unsigned int index);
		void destroy() final;

		_NODISCARD size_t size() const noexcept;
		_NODISCARD unsigned int index() const noexcept;

		void write(size_t offset, size_t size, _In_reads_bytes_(size) const void* data);

		template <typename T>
		void write(size_t offset, const T& data);

		void bind() const final;
		void unbind() const final;

	private:
		size_t m_size = 0;
		unsigned int m_index = -1;
	};
}
