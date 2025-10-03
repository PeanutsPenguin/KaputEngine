#pragma once

#include "Rendering/Buffer/VertexAttributeBuffer.h"

#include "Queue/Context.h"
#include "Rendering/GlTypes.h"

#include <crtdbg.h>
#include <glad/glad.h>

namespace KaputEngine::Rendering::Buffer
{
    template <typename T, typename TVertex>
    void VertexAttributeBuffer::defineAttribute(const unsigned int index, T TVertex::* const attribute, const bool normalized)
    {
        enum class eAttribType
        {
            INTEGER,
            DOUBLE,
            OTHER
        };

        bind();

        uint8_t size = 1;
        GLenum type = Rendering::glType<T>();
        eAttribType attribType = eAttribType::OTHER;

        if constexpr (std::is_class_v<T> && LibMath::IsMathArray<T>)
        {
            size = T::Size;
            type = Rendering::glType<typename T::DataType>();

            if constexpr (std::integral<typename T::DataType>)
                attribType = eAttribType::INTEGER;
            else if constexpr (std::same_as<typename T::DataType, double>)
                attribType = eAttribType::DOUBLE;
        }
        else
        {
            if constexpr (std::integral<T>)
                attribType = eAttribType::INTEGER;
            else if constexpr (std::same_as<T, double>)
                attribType = eAttribType::DOUBLE;
        }

        _ASSERT(type != GL_INVALID_ENUM);

        // Find the offset of the field in the vertex
        // Assuming a vertex object at address null (0), the address of the field is the offset
        // Reinterpreting the field to char to bypass custom operator& that may assume a valid object
        // Based on <cstddef> offsetof - Macro substitution fails with field references
        const void* startPtr = &reinterpret_cast<const char&>(static_cast<TVertex*>(nullptr)->*attribute);

        KaputEngine::Queue::ContextQueue::instance().push([attribType, index, size, type, startPtr, normalized]
        {
            switch (attribType)
            {
            case eAttribType::INTEGER:
                glVertexAttribIPointer(index, size, type, sizeof(TVertex), startPtr);
                break;
            case eAttribType::DOUBLE:
                glVertexAttribLPointer(index, size, type, sizeof(TVertex), startPtr);
                break;
            case eAttribType::OTHER:
                glVertexAttribPointer(index, size, type, normalized, sizeof(TVertex), startPtr);
                break;
            }

            glVertexAttribPointer(index, size, type, normalized, sizeof(TVertex), startPtr);

            glEnableVertexAttribArray(index);
        });
    }
}
