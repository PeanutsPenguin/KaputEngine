#pragma once

#include "Rendering/Buffer/TextureBuffer.h"

namespace KaputEngine::Rendering
{
	struct TextureSample
	{
		std::shared_ptr<const Buffer::TextureBuffer> texture;
		unsigned int unitIndex;
	};
}
