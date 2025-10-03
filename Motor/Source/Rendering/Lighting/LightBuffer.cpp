#include "Rendering/Lighting/LightBuffer.h"

using KaputEngine::Rendering::Buffer::SharedBuffer;
using KaputEngine::Rendering::Lighting::LightBuffer;

const SharedBuffer& LightBuffer::buffer() const noexcept
{
	return m_buffer;
}

void LightBuffer::destroy()
{
	m_buffer.destroy();
}
