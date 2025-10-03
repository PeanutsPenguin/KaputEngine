#include "Rendering/Buffer/FrameBuffer.h"

#include "Queue/Context.h"
#include "Rendering/Buffer/RenderBuffer.h"
#include "Rendering/Buffer/TextureBuffer.h"

#include <glad/glad.h>

using namespace KaputEngine::Rendering::Buffer;

using LibMath::Vector3i;
using KaputEngine::Queue::ContextQueue;

FrameBuffer::~FrameBuffer()
{
	if (m_id)
		destroy();
}

void FrameBuffer::bind() const
{
	ContextQueue::instance().push([this]
	{
		glBindFramebuffer(GL_FRAMEBUFFER, id());
	}).wait();
}

void FrameBuffer::unbind() const
{
	ContextQueue::instance().push([]
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}).wait();
}

void FrameBuffer::destroy()
{
	if (!m_id)
		return;

	ContextQueue::instance().push([id = m_id]
	{
		glDeleteFramebuffers(1, &id);
	});

	m_id = 0;
}

void FrameBuffer::linkTextureBuffer(TextureBuffer& tex)
{
	bind();

	ContextQueue::instance().push([&tex]
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.id(), 0);
	}).wait();

	unbind();
}

void FrameBuffer::linkRenderBuffer(const RenderBuffer& renderObj)
{
	bind();

	ContextQueue::instance().push([&renderObj]
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderObj.id());
	}).wait();

	unbind();
}

void FrameBuffer::resize(const Vector3i& size, TextureBuffer& texId, RenderBuffer& RBO)
{
	texId.resize(size, nullptr);
	this->linkTextureBuffer(texId);

	RBO.resize(size);
	this->linkRenderBuffer(RBO);

	if (!this->checkCompletion())
		std::cerr << __FUNCTION__"Buffer is not complete!\n";
}

void FrameBuffer::create(const Vector3i& size, TextureBuffer& texId, RenderBuffer& RBO)
{
	ContextQueue::instance().push([this]
	{
		glGenFramebuffers(1, const_cast<unsigned int*>(&m_id));
	}).wait();

	this->resize(size, texId, RBO);
}

bool FrameBuffer::checkCompletion()
{
	bind();

	bool result;

	ContextQueue::instance().push([&result]
	{
		result = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}).wait();

	unbind();

	return result;
}
