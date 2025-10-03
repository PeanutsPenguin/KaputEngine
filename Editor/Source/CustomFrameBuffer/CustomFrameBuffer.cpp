#include "CustomFrameBuffer/CustomFrameBuffer.h"

using namespace LibMath;
using namespace KaputEditor;

void CustomFrameBuffer::create(const Vector2i& size)
{
	const Vector3i depthSize = { size.x(), size.y(), 3 };

	this->m_tex.create(depthSize, nullptr);
	this->m_rbo.create(size);
	this->m_fbo.create(depthSize, this->m_tex, this->m_rbo);
}

void CustomFrameBuffer::resize(const Vector2i& size)
{
	this->m_fbo.resize({ size.x(), size.y(), 3 }, this->m_tex, this->m_rbo);
}

unsigned int CustomFrameBuffer::getTexId() const noexcept
{
	return this->m_tex.id();
}

void CustomFrameBuffer::bind()
{
	this->m_fbo.bind();
}

void CustomFrameBuffer::unbind()
{
	this->m_fbo.unbind();
}
