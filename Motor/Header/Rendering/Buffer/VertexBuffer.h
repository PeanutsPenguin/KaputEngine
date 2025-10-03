#pragma once

#include "Buffer.h"

namespace KaputEngine::Rendering::Buffer
{
    class VertexBuffer final : public Buffer
    {
    public:
        VertexBuffer() = default;
		VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&&) noexcept = default;

        ~VertexBuffer() final;

        VertexBuffer& operator=(VertexBuffer&&) = default;

        void create(_In_reads_bytes_(size) const void* data, ptrdiff_t size);

        void bind() const override;
        void unbind() const override;
    };
}
