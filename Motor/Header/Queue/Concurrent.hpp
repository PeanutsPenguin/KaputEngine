#pragma once

#include "Concurrent.h"

#define TEMPLATE template <typename T>
#define CONCURRENTQUEUE ConcurrentQueue<T>

namespace KaputEngine::Queue
{
	TEMPLATE
	void CONCURRENTQUEUE::push(Bind<T> auto&& item)
	{
		std::lock_guard lock(m_mutex);
		m_queue.push(std::forward<decltype(item)>(item));
	}

	TEMPLATE
	template <typename... Args>
	T& CONCURRENTQUEUE::emplace(Args&&... args)
	{
		std::lock_guard lock(m_mutex);
		return m_queue.emplace(std::forward<Args>(args)...);
	}

	TEMPLATE
	bool CONCURRENTQUEUE::pop(_When_(return == true, _Out_) T* item)
	{
		std::lock_guard lock(m_mutex);

		if (!size())
			return false;

		// Move the item to the output ptr and pop
		*item = std::move(m_queue.front());
		m_queue.pop();

		return true;
	}

	TEMPLATE
	size_t CONCURRENTQUEUE::size() const noexcept
	{
		return m_queue.size();
	}
}

#undef TEMPLATE
#undef CONCURRENTQUEUE
