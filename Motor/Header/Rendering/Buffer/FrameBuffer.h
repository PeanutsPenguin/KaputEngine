#pragma once

#include "Buffer.h"

#include <LibMath/Vector/Vector3.h>

namespace KaputEngine::Rendering::Buffer
{
	class RenderBuffer;
	class TextureBuffer;

	class FrameBuffer final : public Buffer
	{
	public:
		FrameBuffer() = default;
		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer(FrameBuffer&&) noexcept = default;

		~FrameBuffer() override;

		FrameBuffer& operator=(FrameBuffer&&) = default;

		void bind() const override;
		void unbind() const override;

		void linkTextureBuffer(TextureBuffer& tex);

		void linkRenderBuffer(const RenderBuffer& renderObj);

		void resize(const LibMath::Vector3i& size, TextureBuffer& texId, RenderBuffer& RBO);

		void create(const LibMath::Vector3i& size, TextureBuffer& texId, RenderBuffer& RBO);
		void destroy() final;

		_NODISCARD bool checkCompletion();
	};
}
