#pragma once

#include "Pointer.h"

#define EXCLUSIVE_TEMPLATE template <typename T>
#define EXCLUSIVE exclusive_weak_ptr<T>

namespace KaputEngine
{
	EXCLUSIVE_TEMPLATE
	EXCLUSIVE::exclusive_weak_ptr(const std::shared_ptr<T>& ptr) noexcept
		: m_ptr(static_cast<std::weak_ptr<T>>(ptr)) { }

	EXCLUSIVE_TEMPLATE
	EXCLUSIVE::exclusive_weak_ptr(const std::weak_ptr<T>& ptr) noexcept
		: m_ptr(ptr) { }

	EXCLUSIVE_TEMPLATE
	EXCLUSIVE::exclusive_weak_ptr(std::weak_ptr<T>&& ptr) noexcept
		: m_ptr(std::move(ptr)) { }

	EXCLUSIVE_TEMPLATE
	bool EXCLUSIVE::expired() const noexcept
	{
		return m_ptr.expired();
	}

	EXCLUSIVE_TEMPLATE
	void EXCLUSIVE::reset() noexcept
	{
		m_ptr.reset();
	}

	EXCLUSIVE_TEMPLATE
	_When_(expired(), _Ret_null_) _When_(!expired(), _Ret_notnull_)
	T* EXCLUSIVE::operator->() & noexcept
	{
		return m_ptr.expired() ? nullptr : std::to_address(m_ptr.lock());
	}

	EXCLUSIVE_TEMPLATE
	_When_(expired(), _Ret_null_) _When_(!expired(), _Ret_notnull_)
	const T* EXCLUSIVE::operator->() const& noexcept
	{
		return const_cast<exclusive_weak_ptr<T>*>(this)->operator->();
	}

	EXCLUSIVE_TEMPLATE
	_Ret_maybenull_ _Ret_maybenull_ T& EXCLUSIVE::operator*() &
	{
		return *operator->();
	}

	EXCLUSIVE_TEMPLATE
	_Ret_maybenull_ const T& EXCLUSIVE::operator*() const&
	{
		return *operator->();
	}

	EXCLUSIVE_TEMPLATE
	template <typename TRight>
	_NODISCARD bool EXCLUSIVE::operator==(const exclusive_weak_ptr<TRight>& right) const noexcept
	{
		return m_ptr.lock() == right.m_ptr.lock();
	}

	EXCLUSIVE_TEMPLATE
	template <typename TRight>
	_NODISCARD bool EXCLUSIVE::operator!=(const exclusive_weak_ptr<TRight>& right) const noexcept
	{
		return m_ptr.lock() != right.m_ptr.lock();
	}

	#pragma region Cast
	EXCLUSIVE_TEMPLATE
	EXCLUSIVE::operator exclusive_weak_ptr<const T>() const noexcept
	{
		return { std::static_pointer_cast<const T>(m_ptr.lock()) };
	}

	EXCLUSIVE_TEMPLATE
	template <std::derived_from<T> Target>
	EXCLUSIVE::operator exclusive_weak_ptr<Target>() const noexcept
	{
		return { std::static_pointer_cast<Target>(m_ptr.lock()) };
	}

	EXCLUSIVE_TEMPLATE
	template <std::derived_from<T> Target>
	EXCLUSIVE::operator exclusive_weak_ptr<const Target>() const noexcept
	{
		return { std::static_pointer_cast<const Target>(m_ptr.lock()) };
	}

	EXCLUSIVE_TEMPLATE
	template <typename Target> requires (std::derived_from<T, Target>)
	EXCLUSIVE::operator exclusive_weak_ptr<Target>() const noexcept
	{
		return { std::static_pointer_cast<Target>(m_ptr.lock()) };
	}

	EXCLUSIVE_TEMPLATE
	template <typename Target> requires (std::derived_from<T, Target>)
	EXCLUSIVE::operator exclusive_weak_ptr<const Target>() const noexcept
	{
		return { std::static_pointer_cast<Target>(m_ptr.lock()) };
	}
	#pragma endregion
}

#undef EXCLUSIVE_TEMPLATE
#undef EXCLUSIVE
