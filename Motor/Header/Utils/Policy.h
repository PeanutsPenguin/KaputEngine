#pragma once

#include "Utils/Function.h"
#include "Utils/rawalloc.h"

#include <cstdint>
#include <functional>
#include <future>
#include <type_traits>

namespace KaputEngine
{
	enum class eMultiThreadPolicy : std::uint8_t
	{
		// Run sequentially
		MONO_THREAD,
		// Run in parallel
		MULTI_THREAD
	};

	template <typename T>
	_NODISCARD std::future<T> emptyFuture()
	{
		std::promise<T> promise;

		if constexpr (std::is_void_v<T>)
			promise.set_value();
		else
			promise.set_value(rawalloc<T, false>().val());

		return promise.get_future();
	}

	template <typename T>
		requires (!std::is_void<T>())
	_NODISCARD std::future<T> emptyFuture(T&& val)
	{
		std::promise<T> promise;
		promise.set_value(std::forward<T>(val));
		return promise.get_future();
	}

	template <typename TResult>
	_NODISCARD std::future<TResult> createFuture(eMultiThreadPolicy policy, std::function<TResult(eMultiThreadPolicy)>&& func)
	{
		switch (policy)
		{
		case eMultiThreadPolicy::MONO_THREAD:
			{
				std::packaged_task<TResult(eMultiThreadPolicy)> task(std::move(func));
				std::future<TResult> future = task.get_future();
				task(policy);

				return future;
			}
		case eMultiThreadPolicy::MULTI_THREAD:
			return std::async(std::launch::async, std::move(func), policy);
		default:
			throw std::runtime_error("Unreachable branch.");
		}
	}
}
