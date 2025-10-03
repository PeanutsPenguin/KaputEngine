#pragma once

#include "PartialBuffer.h"

#include "Assign.h"

#include <stdexcept>

#define TEMPLATE template <typename T, size_t InlineSize>
#define PARTIALBUFFER PartialBuffer<T, InlineSize>

namespace KaputEngine
{
	TEMPLATE
	PARTIALBUFFER::PartialBuffer(size_t size) : m_size(size)
	{
		if (size > InlineSize)
			m_ptr = new T[size];
		else
		{
			m_ptr = m_inlineAlloc.val();

			for (int i = 0; i < size; i++)
				new (m_ptr + i) T();
		}
	}

	TEMPLATE
	PARTIALBUFFER::PartialBuffer(const PartialBuffer& buffer) : m_size(buffer.m_size)
	{
		if (m_size > InlineSize)
			m_ptr = (new rawalloc<T[m_size]>())->ptr();

		for (int i = 0; i < m_size; i++)
			new (m_ptr + i) T(buffer.m_ptr[i]);
	}

	
	TEMPLATE
	PARTIALBUFFER::PartialBuffer(PartialBuffer&& buffer) noexcept
		: m_ptr(buffer.m_ptr), m_size(buffer.m_size)
	{
		if (m_size > InlineSize)
		{
			m_ptr = buffer.m_ptr;
			buffer.m_ptr = nullptr;
		}
		else
			for (int i = 0; i < m_size; i++)
				new (m_ptr + i) T(std::move(buffer.m_ptr[i]));
	}

	TEMPLATE
	PARTIALBUFFER::~PartialBuffer()
	{
		if (m_size > InlineSize)
		{
			delete[] m_ptr;
			m_ptr = nullptr;
		}
		else
			for (int i = 0; i < m_size; i++)
				m_ptr[i].~T();
	}

	TEMPLATE
	PARTIALBUFFER& PARTIALBUFFER::operator=(const PartialBuffer& buffer)
	{
		return copyAssign(*this, buffer);
	}

	TEMPLATE
	PARTIALBUFFER& PARTIALBUFFER::operator=(PartialBuffer&& buffer) noexcept
	{
		return moveAssign(*this, buffer);
	}

	TEMPLATE
	size_t PARTIALBUFFER::size() const noexcept
	{
		return m_size;
	}

	TEMPLATE
	consteval size_t PARTIALBUFFER::inlineSize() noexcept
	{
		return InlineSize;
	}

	TEMPLATE
	T* PARTIALBUFFER::data() noexcept
	{
		return m_ptr;
	}

	TEMPLATE
	const T* PARTIALBUFFER::data() const noexcept
	{
		return m_ptr;
	}

	TEMPLATE
	PARTIALBUFFER::operator T* () noexcept
	{
		return *m_ptr;
	}

	TEMPLATE
	PARTIALBUFFER::operator const T* () const noexcept
	{
		return *m_ptr;
	}

	TEMPLATE
	T& PARTIALBUFFER::operator*() noexcept
	{
		return *m_ptr;
	}

	TEMPLATE
	const T& PARTIALBUFFER::operator*() const noexcept
	{
		return *m_ptr;
	}

	TEMPLATE
	T& PARTIALBUFFER::operator[](int index)
	{
#ifdef _DEBUG
		if (index >= m_size)
			throw std::out_of_range("index");
#endif // _DEBUG

		return m_ptr[index];
	}

	TEMPLATE
	const T& PARTIALBUFFER::operator[](int index) const
	{
		return const_cast<PartialBuffer*>(this)[index];
	}
}

#undef TEMPLATE
#undef PARTIALBUFFER
