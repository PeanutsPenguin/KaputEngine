#include "Rendering/Buffer/ElementBuffer.h"

#include "Queue/Context.h"

#include <glad/glad.h>

using KaputEngine::Queue::ContextQueue;
using KaputEngine::Rendering::Buffer::ElementBuffer;

ElementBuffer::~ElementBuffer()
{
	destroy();
}

void ElementBuffer::create(const std::span<const unsigned int>& indices)
{
	m_count = indices.size();

	generateBuffer();
	bind();

	ContextQueue::instance().push([this, &indices]
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);
	}).wait();
}

void ElementBuffer::bind() const
{
	ContextQueue::instance().push([this]
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}).wait();
}

void ElementBuffer::unbind() const
{
	ContextQueue::instance().push([]
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}).wait();
}

const int& ElementBuffer::count() const noexcept
{
	return m_count;
}
