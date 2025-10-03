#include "Rendering/Buffer/RenderBuffer.h"

#include "Queue/Context.h"

#include <glad/glad.h>

using LibMath::Vector2i;
using KaputEngine::Queue::ContextQueue;
using KaputEngine::Rendering::Buffer::RenderBuffer;

RenderBuffer::~RenderBuffer()
{
	destroy();
}

void RenderBuffer::bind() const
{
	ContextQueue::instance().push([this]()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, id());
	}).wait();
}
void RenderBuffer::unbind() const
{
	ContextQueue::instance().push([]()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}).wait();
}

void RenderBuffer::create(const Vector2i& size)
{
	ContextQueue::instance().push([this]()
	{
		glGenRenderbuffers(1, &m_id);
	}).wait();

	this->resize(size);
}

void RenderBuffer::destroy()
{
	if (!m_id)
		return;

	ContextQueue::instance().push([id = m_id]
	{
		glDeleteRenderbuffers(1, &id);
	});

	m_id = 0;
}


void RenderBuffer::resize(const Vector2i& size)
{
	bind();

	ContextQueue::instance().push([this, size]()
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x(), size.y());
	}).wait();

	unbind();
}
