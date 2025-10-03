#pragma once

#include "Buffer.h"

#include <LibMath/Vector/Vector2.h>

namespace KaputEngine::Rendering::Buffer
{
	 class RenderBuffer final: public Buffer
	 {
	 public:
		 RenderBuffer() = default;
		 RenderBuffer(const RenderBuffer&) = delete;
		 RenderBuffer(RenderBuffer&&) noexcept = default;

		 ~RenderBuffer() final;

		 RenderBuffer& operator=(RenderBuffer&&) = default;

		 void bind() const final;
		 void unbind() const final;

		 void create(const LibMath::Vector2i& size);
		 void resize(const LibMath::Vector2i& size);

		 void destroy() final;
	 };
}
