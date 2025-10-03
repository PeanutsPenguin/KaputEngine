#pragma once

#include "Queue/Concurrent.h"

#include <functional>
#include <future>
#include <thread>
#include <type_traits>

namespace KaputEngine::Queue
{
	/// <summary>
	/// Thread-safe queue of functions to be executed by an owning thread
	/// </summary>
	/// <typeparam name="Func">Contained function signature</typeparam>
	template <typename Func>
		requires std::is_function_v<Func>
	class ActionQueue
	{
	public:
		using Function = Func;
		using Lambda = std::function<Func>;
		using Return = typename Lambda::result_type;

		/// <summary>
		/// Container type for queue items
		/// </summary>
		class Item
		{
			friend ActionQueue;

		public:
			Item(Lambda&& func);

		private:
			// Function to be executed
			Lambda m_func;

			// Promise wrapping the async execution
			std::promise<Return> m_promise;

			/// <summary>
			/// Invokes the function and sets the promise.
			/// </summary>
			/// <typeparam name="args">Function arguments</typeparam>
			template <typename... Args>
			void invoke(Args&&... args);
		};

		ActionQueue();
		ActionQueue(const ActionQueue&) = delete;
		ActionQueue(ActionQueue&&) = delete;

		~ActionQueue() = default;

		ActionQueue& operator=(const ActionQueue&) = delete;
		ActionQueue& operator=(ActionQueue&&) = delete;

		/// <summary>
		/// Returns the id of the thread owning the queue.
		/// </summary>
		_NODISCARD std::thread::id owner() const noexcept;

		_NODISCARD bool validateThread() const noexcept;

		/// <summary>
		/// Pushes a function to the queue.
		/// </summary>
		/// <param name="allowRunImmediate">If the calling thread is the owning thread, the function can be executed immediately using the given arguments, bypassing the queue</param>
		/// <param name="args">Function arguments for immediate execution</param>
		template <typename... Args>
		std::future<Return> push(Lambda&& func, const bool allowRunImmediate = true, Args&&... args);

		/// <summary>
		/// Executed and pops a function from the queue.
		/// </summary>
		/// <param name="args">Function arguments</param>
		/// <exception cref="logic_error">The calling thread is not the owning thread of the queue.</exception>
		template <typename... Args>
		bool pop(Args&&... args);

		/// <summary>
		/// Executes and pops all functions from the queue.
		/// </summary>
		/// <param name="args">Function arguments</param>
		/// <exception cref="logic_error">The calling thread is not the owning thread of the queue.</exception>
		template <typename... Args>
		void popAll(Args&&... args);

	protected:
		ConcurrentQueue<Item> m_queue;
		std::thread::id m_owner;
	};
}

#include "Action.hpp"
