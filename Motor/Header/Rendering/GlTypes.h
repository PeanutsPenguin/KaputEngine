#pragma once

#include <LibMath/MathArray/Concepts.h>
#include <LibMath/MathArray/Internal/Macros.h>

#include <glad/glad.h>
#include <vcruntime.h>

namespace KaputEngine::Rendering
{
    using RenderBool = uint32_t;

    class Color;

    namespace Rendering
    {
        template <typename T>
        _NODISCARD consteval GLenum glType() noexcept
        {
            if constexpr (LibMath::IsMathArray<T>)
            	return glType<typename T::template StaticAs<LibMath::MathArray>>();
            else
                return GL_INVALID_ENUM;
        }

#define GL_TYPE_DEF(type, gl_type) \
template <> _NODISCARD consteval GLenum glType<type>() noexcept \
{ return gl_type; }

#define GL_TYPE_DEF_VECS(type, gl_type) \
GL_TYPE_DEF(ARGS(LibMath::MathArray<1, 2, type>), gl_type##_VEC2) \
GL_TYPE_DEF(ARGS(LibMath::MathArray<1, 3, type>), gl_type##_VEC3) \
GL_TYPE_DEF(ARGS(LibMath::MathArray<1, 4, type>), gl_type##_VEC4) \

#define GL_TYPE_DEF_MATS(type, gl_type) \
GL_TYPE_DEF(ARGS(LibMath::MathArray<2, 2, type>), gl_type##_MAT2) \
GL_TYPE_DEF(ARGS(LibMath::MathArray<2, 3, type>), gl_type##_MAT2x3) \
GL_TYPE_DEF(ARGS(LibMath::MathArray<2, 4, type>), gl_type##_MAT2x4) \
GL_TYPE_DEF(ARGS(LibMath::MathArray<3, 2, type>), gl_type##_MAT3x2) \
GL_TYPE_DEF(ARGS(LibMath::MathArray<3, 3, type>), gl_type##_MAT3) \
GL_TYPE_DEF(ARGS(LibMath::MathArray<3, 4, type>), gl_type##_MAT3x4) \
GL_TYPE_DEF(ARGS(LibMath::MathArray<4, 2, type>), gl_type##_MAT4x2) \
GL_TYPE_DEF(ARGS(LibMath::MathArray<4, 3, type>), gl_type##_MAT4x3) \
GL_TYPE_DEF(ARGS(LibMath::MathArray<4, 4, type>), gl_type##_MAT4) \

        GL_TYPE_DEF(signed char, GL_BYTE)
        GL_TYPE_DEF(unsigned char, GL_UNSIGNED_BYTE)
        GL_TYPE_DEF(short, GL_SHORT)
        GL_TYPE_DEF(unsigned short, GL_UNSIGNED_SHORT)
        GL_TYPE_DEF(int, GL_INT)
        GL_TYPE_DEF(unsigned int, GL_UNSIGNED_INT)
        GL_TYPE_DEF(float, GL_FLOAT)
        GL_TYPE_DEF(double, GL_DOUBLE)
        GL_TYPE_DEF(bool, GL_BOOL)

        GL_TYPE_DEF_VECS(int, GL_INT)
        GL_TYPE_DEF_VECS(unsigned int, GL_UNSIGNED_INT)
        GL_TYPE_DEF_VECS(float, GL_FLOAT)
        GL_TYPE_DEF_VECS(double, GL_DOUBLE)

        GL_TYPE_DEF_MATS(float, GL_FLOAT)
        GL_TYPE_DEF_MATS(double, GL_DOUBLE)

#undef GL_TYPE_DEF
#undef GL_TYPE_DEF_VECS
#undef GL_TYPE_DEF_MATS
    }
}
