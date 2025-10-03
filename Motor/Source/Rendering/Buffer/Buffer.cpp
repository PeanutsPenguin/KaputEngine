#include "Rendering/Buffer/Buffer.h"

#include "Queue/Context.h"

#include <glad/glad.h>

using KaputEngine::Queue::ContextQueue;
using KaputEngine::Rendering::Buffer::Buffer;

void Buffer::destroy()
{
	if (!m_id)
		return;

	ContextQueue::instance().push([id = m_id]
	{
		glDeleteBuffers(1, &id);
	});

	m_id = 0;
}

const unsigned int& Buffer::id() const noexcept
{
	return m_id;
}

bool Buffer::valid() const noexcept
{
	return m_id;
}

void Buffer::generateBuffer()
{
	ContextQueue::instance().push([this]
	{
		glGenBuffers(1, &m_id);
	}).wait();
}
