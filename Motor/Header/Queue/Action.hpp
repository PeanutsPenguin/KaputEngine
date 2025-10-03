#pragma once

#include "Action.h"

#include "Utils/Policy.h"

#include <exception>
#include <iostream>
#include <stdexcept>

#define ACTIONQUEUE ActionQueue<Func>
#define ACTIONQUEUE_TEMPLATE template <typename Func> requires std::is_function_v<Func>

namespace KaputEngine::Queue
{
#pragma region Item
	ACTIONQUEUE_TEMPLATE
	ACTIONQUEUE::Item::Item(Lambda&& func)
		: m_func(std::move(func)) { }

	ACTIONQUEUE_TEMPLATE
	template <typename... Args>
	void ACTIONQUEUE::Item::invoke(Args&&... args)
	{
		try
		{
			if constexpr (std::is_void_v<Return>)
			{
				// Placing the invoke call as a set_value parameter is illegal for void
				m_func(std::forward<Args>(args)...);

				m_promise.set_value();
			}
			else
				m_promise.set_value(m_func(std::forward<Args>(args)...));
		} catch (...)
		{
			// On exception, forward the exception in the promise
			m_promise.set_exception(std::current_exception());
		}
	}
#pragma endregion

#pragma region ActionQueue
	ACTIONQUEUE_TEMPLATE
	ACTIONQUEUE::ActionQueue()
		: m_owner(std::this_thread::get_id()) { }

	ACTIONQUEUE_TEMPLATE
	std::thread::id ACTIONQUEUE::owner() const noexcept
	{
		return m_owner;
	}

	ACTIONQUEUE_TEMPLATE
	template <typename... Args>
	std::future<typename ACTIONQUEUE::Return> ACTIONQUEUE::push(Lambda&& func, const bool allowRunImmediate, Args&&... args)
	{
		// TODO Find a way to bind the arguments to the function signature instead of relying on loose variadic arguments

		if (!allowRunImmediate || std::this_thread::get_id() != m_owner)
			// Not the owning thread - Queue the function
			return m_queue.emplace(std::move(func)).m_promise.get_future();

		if constexpr (std::is_void_v<Return>)
		{
			func(std::forward<Args>(args)...);
			return emptyFuture<void>();
		}
		else
			return emptyFuture<Return>(func(std::forward<Args>(args)...));
	}

	ACTIONQUEUE_TEMPLATE
	template <typename... Args>
	bool ACTIONQUEUE::pop(Args&&... args)
	{
		// Validate calling thread
		if (std::this_thread::get_id() != m_owner)
			throw std::logic_error("Only the owning thread can pop actions from the queue.");

		// Allocate a raw container on the stack as there is no empty ctor for Item
		rawalloc<Item> actionAlloc;

		if (!m_queue.pop(actionAlloc.ptr()))
			return false;

		actionAlloc->invoke(std::forward<Args>(args)...);
		return true;
	}

	ACTIONQUEUE_TEMPLATE
	template <typename... Args>
	void ACTIONQUEUE::popAll(Args&&... args)
	{
		while (pop(std::forward<Args>(args)...)) {}
	}

	ACTIONQUEUE_TEMPLATE
	bool ACTIONQUEUE::validateThread() const noexcept
	{
		if (std::this_thread::get_id() != m_owner)
		{
			std::cerr << __FUNCTION__"Thread " << std::this_thread::get_id() << " is not the owner of the queue.\n";
			return false;
		}

		return true;
	}
#pragma endregion
}

#undef ACTIONQUEUE
#undef ACTIONQUEUE_TEMPLATE
