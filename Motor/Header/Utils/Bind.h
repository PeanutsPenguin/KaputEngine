#pragma once

#include <concepts>

namespace KaputEngine
{
	/// <summary>
	/// Constraint for type-safe universal references.
	/// </summary>
	/// <remarks>Prevents the usage of implicit conversions.</remarks>
	template <typename T, typename TBinding>
	concept Bind = std::constructible_from<TBinding, T>;

	template <typename T, typename TBinding>
	using BindingResult = std::conditional_t<
		std::same_as<T, TBinding&>,
		TBinding&,
		std::conditional_t<
		std::same_as<T, TBinding>,
		TBinding&&,
		TBinding>>;

	template <typename TBinding, typename T>
	BindingResult<T, TBinding> bindGet(T&& binding)
	{
		if constexpr (std::same_as<T, TBinding&>)
			return binding;
		else if constexpr (std::same_as<T, TBinding>)
			return std::move(binding);
		else
			return TBinding(std::forward<T>(binding));
	}
}

#define FORWARD(value) std::forward<decltype(value)>(value)
