#include "Rendering/Buffer/VertexAttributeBuffer.h"

#include "Queue/Context.h"

#include <glad/glad.h>

using KaputEngine::Rendering::Buffer::VertexAttributeBuffer;
using KaputEngine::Queue::ContextQueue;

VertexAttributeBuffer::~VertexAttributeBuffer()
{
	destroy();
}

void VertexAttributeBuffer::create()
{
	ContextQueue::instance().push([this]
	{
		glGenVertexArrays(1, &m_id);
	}).wait();
}

void VertexAttributeBuffer::destroy()
{
	if (!m_id)
		return;

	ContextQueue::instance().push([id = m_id]
	{
		glDeleteVertexArrays(1, &id);
	});

	m_id = 0;
}

void VertexAttributeBuffer::bind() const
{
	ContextQueue::instance().push([this]
	{
		glBindVertexArray(m_id);
	}).wait();
}

void VertexAttributeBuffer::unbind() const
{
	ContextQueue::instance().push([]
	{
		glBindVertexArray(0);
	}).wait();
}
