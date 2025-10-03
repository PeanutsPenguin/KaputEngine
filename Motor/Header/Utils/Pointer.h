#pragma once

#include <memory>
#include <type_traits>

namespace KaputEngine
{
	/// <summary>
	/// Type alias for representing raw pointers in template form
	/// </summary>
	/// <typeparam name="T">Item type</typeparam>
	template <typename T>
	using RawPtr = T*;

	/// <summary>
	/// Constraint for various raw or smart pointer types
	/// </summary>
	template <typename Ptr, typename T = std::remove_reference_t<decltype(*std::declval<Ptr>())>>
	concept IsPointer = requires (Ptr ptr)
	{
		{ Ptr() } -> std::same_as<Ptr>;
		{ *std::declval<Ptr>() } -> std::same_as<T&>;
		{ ptr = nullptr } -> std::same_as<Ptr&>;
		{ ptr == nullptr } -> std::convertible_to<bool>;
		{ ptr != nullptr } -> std::convertible_to<bool>;
		{ static_cast<bool>(ptr) };
		{ std::to_address(ptr) } -> std::same_as<T*>;
	};

	template <typename T>
	class exclusive_weak_ptr
	{
		template <typename T>
		friend class exclusive_weak_ptr;

	public:
		exclusive_weak_ptr() noexcept = default;
		exclusive_weak_ptr(const exclusive_weak_ptr&) noexcept = default;
		exclusive_weak_ptr(exclusive_weak_ptr&&) noexcept = default;
		exclusive_weak_ptr(const std::shared_ptr<T>& ptr) noexcept;
		exclusive_weak_ptr(const std::weak_ptr<T>& ptr) noexcept;
		exclusive_weak_ptr(std::weak_ptr<T>&& ptr) noexcept;

		~exclusive_weak_ptr() = default;

		_NODISCARD bool expired() const noexcept;

		void reset() noexcept;

		exclusive_weak_ptr& operator=(const exclusive_weak_ptr&) noexcept = default;
		exclusive_weak_ptr& operator=(exclusive_weak_ptr&&) noexcept = default;

		_NODISCARD _When_(expired(), _Ret_null_) _When_(!expired(), _Ret_notnull_) T* operator->() & noexcept;
		_NODISCARD _When_(expired(), _Ret_null_) _When_(!expired(), _Ret_notnull_) const T* operator->() const& noexcept;

		_NODISCARD T& operator*() &;
		_NODISCARD const T& operator*() const&;

		template <typename TRight>
		_NODISCARD bool operator==(const exclusive_weak_ptr<TRight>& right) const noexcept;

		template <typename TRight>
		_NODISCARD bool operator!=(const exclusive_weak_ptr<TRight>& right) const noexcept;

		#pragma region Cast
		_NODISCARD operator exclusive_weak_ptr<const T>() const noexcept;

		template <std::derived_from<T> Target>
		_NODISCARD explicit operator exclusive_weak_ptr<Target>() const noexcept;

		template <std::derived_from<T> Target>
		_NODISCARD explicit operator exclusive_weak_ptr<const Target>() const noexcept;

		template <typename Target> requires (std::derived_from<T, Target>)
		_NODISCARD operator exclusive_weak_ptr<Target>() const noexcept;

		template <typename Target> requires (std::derived_from<T, Target>)
		_NODISCARD operator exclusive_weak_ptr<const Target>() const noexcept;
		#pragma endregion

	private:
		std::weak_ptr<T> m_ptr { };
	};
}

#include "Pointer.hpp"
