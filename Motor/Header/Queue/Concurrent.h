#pragma once

#include "Utils/Bind.h"

#include <mutex>
#include <queue>
#include <sal.h>
#include <xutility>

namespace KaputEngine::Queue
{
	/// <summary>
	/// Thread-safe queue of items
	/// </summary>
	template <typename T>
	class ConcurrentQueue
	{
	public:
		using Type = T;

		ConcurrentQueue() = default;
		ConcurrentQueue(const ConcurrentQueue&) = delete;
		ConcurrentQueue(ConcurrentQueue&&) = delete;

		~ConcurrentQueue() = default;

		ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;
		ConcurrentQueue& operator=(ConcurrentQueue&&) = delete;

		void push(Bind<T> auto&& item);

		template <typename... Args>
		T& emplace(Args&&... args);

		_NODISCARD bool pop(_When_(return == true, _Out_) T* item);

		_NODISCARD size_t size() const noexcept;

	private:
		std::queue<T> m_queue;
		std::mutex m_mutex;
	};
}

#include "Concurrent.hpp"
