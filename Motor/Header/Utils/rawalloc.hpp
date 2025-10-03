#pragma once

#include "rawalloc.h"

#define TEMPLATE template <typename T, bool Managed>
#define RAWALLOC rawalloc<T, Managed>

namespace KaputEngine
{
	TEMPLATE
	RAWALLOC::rawalloc() requires (Managed) : buffer() { }

	TEMPLATE
	RAWALLOC::~rawalloc()
	{
		if constexpr (Managed)
			this->val().~T();
	}

	TEMPLATE
	T* RAWALLOC::ptr() noexcept
	{
		return reinterpret_cast<T*>(buffer);
	}

	TEMPLATE
	const T* RAWALLOC::ptr() const noexcept
	{
		return reinterpret_cast<const T*>(buffer);
	}

	TEMPLATE
	T& RAWALLOC::val() noexcept
	{
		return *reinterpret_cast<T*>(buffer);
	}

	TEMPLATE
	const T& RAWALLOC::val() const noexcept
	{
		return *reinterpret_cast<T*>(buffer);
	}

	TEMPLATE
	RAWALLOC::operator T& () noexcept
	{
		return this->val();
	}

	TEMPLATE
	RAWALLOC::operator const T& () const noexcept
	{
		return this->val();
	}

	TEMPLATE
	T* RAWALLOC::operator->() noexcept
	{
		return ptr();
	}

	TEMPLATE
	const T* RAWALLOC::operator->() const noexcept
	{
		return ptr();
	}

	TEMPLATE
	RAWALLOC& RAWALLOC::operator=(const T& value)
	{
		this->val() = value;
		return *this;
	}

	TEMPLATE
	RAWALLOC& RAWALLOC::operator=(T&& value)
	{
		this->val() = std::move(value);
		return *this;
	}
}

#undef TEMPLATE
#undef RAWALLOC
