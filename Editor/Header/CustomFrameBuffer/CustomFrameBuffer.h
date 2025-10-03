#pragma once

#include "Rendering/Buffer/FrameBuffer.h"
#include "Rendering/Buffer/RenderBuffer.h"
#include "Rendering/Buffer/TextureBuffer.h"

#include <LibMath/Vector/Vector2.h>

namespace KaputEditor
{
	class CustomFrameBuffer
	{
	public:
		void create(const LibMath::Vector2i& size);
		void resize(const LibMath::Vector2i& size);

		_NODISCARD unsigned int getTexId() const noexcept;

		void bind();

		void unbind();

		~CustomFrameBuffer() = default;

	private:
		KaputEngine::Rendering::Buffer::TextureBuffer m_tex;
		KaputEngine::Rendering::Buffer::FrameBuffer	m_fbo;
		KaputEngine::Rendering::Buffer::RenderBuffer m_rbo;
	};
}
