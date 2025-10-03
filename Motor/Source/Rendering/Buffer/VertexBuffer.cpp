#include "Rendering/Buffer/VertexBuffer.h"

#include "Queue/Context.h"

#include <glad/glad.h>
#include <iostream>

using KaputEngine::Queue::ContextQueue;
using KaputEngine::Rendering::Buffer::VertexBuffer;

VertexBuffer::~VertexBuffer()
{
    destroy();
}

void VertexBuffer::create(_In_reads_bytes_(size) const void* data, const ptrdiff_t size)
{
    generateBuffer();
    bind();

    ContextQueue::instance().push([this, size, data]
    {
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

        GLint createdSize = 0;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &createdSize);

        if (createdSize != size)
            std::cerr << "Buffer data not loaded correctly\n";
    }).wait();
}

void VertexBuffer::bind() const
{
    ContextQueue::instance().push([this]
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
    }).wait();
}

void VertexBuffer::unbind() const
{
    ContextQueue::instance().push([]
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }).wait();
}
