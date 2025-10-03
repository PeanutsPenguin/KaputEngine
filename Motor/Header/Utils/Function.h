#pragma once

#include <functional>

namespace KaputEngine
{
	template <typename T, typename Ret, typename... Args>
	concept Callable = requires (T func, Args&&... args)
	{
		std::invocable<T, Args...>;
		// Possibly loosen to allow covariant return
		{
			func(args...)
		} -> std::same_as<Ret>;
	};

	template <typename Func>
	concept NoArgsFunc =
		std::is_function_v<Func> &&
		std::same_as<std::function<Func>, std::function<std::invoke_result_t<Func>()>>;

	template <typename Return, typename... Args>
	struct function_traits { };

	template <typename Return, typename... Args>
	struct function_traits<Return(Args...)>
	{
		using signature = Return(Args...);
		using return_type = Return;
		using argument_types = std::tuple<Args...>;
		static constexpr size_t argument_count = sizeof...(Args);

		template <size_t Index>
		using argument_at = std::tuple_element_t<Index, argument_types>;
	};

	template <typename Return, typename... Args>
	struct function_traits<Return(*)(Args...)> : function_traits<Return(Args...)> { };

	template <typename Return, typename... Args>
	struct function_traits<Return(&)(Args...)> : function_traits<Return(Args...)> { };

	template <typename Return, typename... Args>
	struct function_traits<std::function<Return(Args...)>> : function_traits<Return(Args...)> { };

	template <typename Return, typename Source, typename... Args>
	struct function_traits<Return(Source::*)(Args...)> : function_traits<Return(Args...)> { };

	template <typename Lambda>
	struct function_traits<Lambda> : function_traits<decltype(std::function(std::declval<Lambda>()))> { };
}
