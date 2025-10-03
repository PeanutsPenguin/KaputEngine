#pragma once

#include "Rendering/Buffer/Buffer.h"

#include <vcruntime.h>

namespace KaputEngine::Rendering::Buffer
{
    class VertexAttributeBuffer final : public Buffer
    {
    public:
        VertexAttributeBuffer() = default;
		VertexAttributeBuffer(const VertexAttributeBuffer&) = delete;
        VertexAttributeBuffer(VertexAttributeBuffer&&) noexcept = default;

        ~VertexAttributeBuffer() final;

        VertexAttributeBuffer& operator=(VertexAttributeBuffer&&) = default;

        void create();
        void destroy() final;

        void bind() const final;
        void unbind() const final;

        template <typename T, typename TVertex>
        void defineAttribute(const unsigned int index, T TVertex::* attribute, bool normalized = false);
    };
}
