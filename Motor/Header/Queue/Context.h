#pragma once

#include "Queue/Action.h"

namespace KaputEngine::Queue
{
	/// <summary>
	/// Singleton queue for actions that need to be executed in the main thread
	/// </summary>
	class ContextQueue : public ActionQueue<void()>
	{
	public:
		_NODISCARD static ContextQueue& instance() noexcept;

	private:
		ContextQueue() = default;
		static ContextQueue m_inst;
	};

	using ContextAction = ContextQueue::Item;
}
