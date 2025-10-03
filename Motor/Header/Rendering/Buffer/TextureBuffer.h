#pragma once

#include "Buffer.h"

#include "Rendering/GlTypes.h"

#include <LibMath/Vector/Vector3.h>

namespace KaputEngine::Resource
{
	class TextureResource;
}

namespace KaputEngine::Rendering::Buffer
{
	class TextureBuffer	 final : public Buffer
	{
	public:
		TextureBuffer() = default;
		TextureBuffer(Resource::TextureResource& parent);
		TextureBuffer(const TextureBuffer&) = delete;
		TextureBuffer(TextureBuffer&&) noexcept = default;

		~TextureBuffer() override;

		TextureBuffer& operator=(TextureBuffer&&) = default;

		void destroy() final;

		void bind() const override;
		void unbind() const override;

		void activate(unsigned int unitIndex) const;

		/// <summary>
		/// Creates the texture buffer with data
		/// </summary>
		/// <param name="size">3D vector with the 2D dimensions as xy and the channel count as z</param>
		/// <param name="data">Texture data</param>
		void create(const LibMath::Vector3i& size, _In_reads_(size.product()) const void* data,
			unsigned int type = Rendering::glType<unsigned char>());

		void resize(const LibMath::Vector3i& size, _In_reads_opt_(size.product()) const void* data);

		_NODISCARD _Ret_maybenull_ Resource::TextureResource* parentResource() noexcept;
		_NODISCARD _Ret_maybenull_ const Resource::TextureResource* parentResource() const noexcept;

	private:
		Resource::TextureResource* m_resource = nullptr;
		unsigned int m_type;
	};
}
