#pragma once

#include "SharedBuffer.h"

template <typename T>
void KaputEngine::Rendering::Buffer::SharedBuffer::write(size_t offset, const T& data)
{
	write(offset, sizeof(T), &data);
}
