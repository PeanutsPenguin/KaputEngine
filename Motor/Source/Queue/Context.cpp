#include "Queue/Context.h"

using KaputEngine::Queue::ContextQueue;

ContextQueue ContextQueue::m_inst;

ContextQueue& ContextQueue::instance() noexcept
{
	return m_inst;
}
