#include "Rendering/Buffer/TextureBuffer.h"

#include "Queue/Context.h"

#include <glad/glad.h>

using LibMath::Vector3i;
using KaputEngine::Rendering::Buffer::TextureBuffer;
using KaputEngine::Resource::TextureResource;
using KaputEngine::Queue::ContextQueue;

TextureBuffer::TextureBuffer(TextureResource& parent) : m_resource(&parent) { }

TextureBuffer::~TextureBuffer()
{
	if (m_id)
		destroy();
}

void TextureBuffer::destroy()
{
	ContextQueue::instance().push([id = m_id]
	{
		glDeleteTextures(1, &id);
	});

	m_id = 0;
}

void TextureBuffer::bind() const
{
	ContextQueue::instance().push([this]
	{
		glBindTexture(GL_TEXTURE_2D, m_id);
	}).wait();
}

void TextureBuffer::unbind() const
{
	ContextQueue::instance().push([]
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}).wait();
}

void TextureBuffer::activate(const unsigned int unitIndex) const
{
	ContextQueue::instance().push([unitIndex]
	{
		glActiveTexture(GL_TEXTURE0 + unitIndex);
	}).wait();

	bind();
}

void TextureBuffer::create(const Vector3i& size, _In_reads_(size.product()) const void* data, const unsigned int type)
{
	m_type = type;

	ContextQueue::instance().push([this]
	{
		glGenTextures(1, &m_id);
	}).wait();

	this->resize(size, data);
}

void TextureBuffer::resize(const Vector3i& size, _In_reads_opt_(size.product()) const void* data)
{
	GLint format;

	switch (size.z())
	{
	case 1:
		format = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		format = GL_INVALID_ENUM;
	}

	bind();

	ContextQueue::instance().push([this, format, size, data]
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, size.x(), size.y(), 0, format, m_type, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}).wait();

	unbind();
}

_Ret_maybenull_ TextureResource* TextureBuffer::parentResource() noexcept
{
	return m_resource;
}

_Ret_maybenull_ const TextureResource* TextureBuffer::parentResource() const noexcept
{
	return m_resource;
}
