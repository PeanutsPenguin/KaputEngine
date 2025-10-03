#pragma once

#include "rawalloc.h"

#include <vcruntime.h>

namespace KaputEngine
{
	/// <summary>
	/// Buffer implementing a stack allocation for sizes up to a given limit. Allocates on the heap if the requested size is larger.
	/// </summary>
	/// <typeparam name="T">Type of items</typeparam>
	/// <typeparam name="InlineSize">Number of items in the stack allocation</typeparam>
	template <typename T, size_t InlineSize>
	class PartialBuffer
	{
	public:
		/// <summary>
		/// Constructs the buffer with the fixed actual size.
		/// </summary>
		/// <param name="size">Actual size. Allocates on the heap if larger than the inline size</param>
		PartialBuffer(size_t size);
		PartialBuffer(const PartialBuffer& buffer);
		PartialBuffer(PartialBuffer&& buffer) noexcept;

		~PartialBuffer();

		PartialBuffer& operator=(const PartialBuffer& buffer);
		PartialBuffer& operator=(PartialBuffer&& buffer) noexcept;

		_NODISCARD size_t size() const noexcept;
		_NODISCARD static consteval size_t inlineSize() noexcept;

		_NODISCARD T* data() noexcept;
		_NODISCARD const T* data() const noexcept;

		_NODISCARD operator T* () noexcept;
		_NODISCARD operator const T* () const noexcept;

		_NODISCARD T& operator *() noexcept;
		_NODISCARD const T& operator *() const noexcept;

		_NODISCARD T& operator[](int index);
		_NODISCARD const T& operator[](int index) const;

	private:
		T* m_ptr;
		size_t m_size;
		rawalloc<T[InlineSize], false> m_inlineAlloc;
	};
}
