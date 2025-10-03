#pragma once

#include <vcruntime.h>

namespace KaputEngine::Rendering::Buffer
{
	class Buffer
	{
	public:
		Buffer() = default;
		Buffer(Buffer&) = delete;
		Buffer(Buffer&&) noexcept = default;

		virtual ~Buffer() = default;

		virtual void destroy();

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		_NODISCARD const unsigned int& id() const noexcept;
		_NODISCARD bool valid() const noexcept;

	protected:
		unsigned int m_id = 0;

		void generateBuffer();
	};
}