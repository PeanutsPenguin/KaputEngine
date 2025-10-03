#include "Rendering/Buffer/SharedBuffer.h"

#include "Queue/Context.h"

#include <glad/glad.h>
#include <iostream>

using KaputEngine::Queue::ContextQueue;
using KaputEngine::Rendering::Buffer::SharedBuffer;

SharedBuffer::~SharedBuffer()
{
	destroy();
}

void SharedBuffer::create(const size_t size, const unsigned int index)
{
	generateBuffer();

	m_size  = size;
	m_index = index;

	bind();

	ContextQueue::instance().push([size]()
	{
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}).wait();

	unbind();
}

void SharedBuffer::destroy()
{
	Buffer::destroy();

	m_size = 0;
	m_index = -1;
}

void SharedBuffer::bind() const
{
	ContextQueue::instance().push([this]()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_id);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_index, m_id);
	}).wait();
}

void SharedBuffer::unbind() const
{
	ContextQueue::instance().push([]()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}).wait();
}

size_t SharedBuffer::size() const noexcept
{
	return m_size;
}

unsigned int SharedBuffer::index() const noexcept
{
	return m_index;
}

void SharedBuffer::write(const size_t offset, const size_t size, _In_reads_bytes_(size) const void* const data)
{
	if (offset + size > m_size)
	{
		std::cerr << "SharedBuffer::write: Attempting to write outside of buffer bounds\n";
		return;
	}

	bind();

	ContextQueue::instance().push([this, offset, size, data]()
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
	}).wait();

	unbind();
}
