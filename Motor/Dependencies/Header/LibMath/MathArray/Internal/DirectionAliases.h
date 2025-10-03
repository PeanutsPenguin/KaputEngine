#pragma once

#include "../Concepts.h"

#define DIRECTION_ALIAS_FUNC(name, size, sign) \
static constexpr T name(typename T::DataType mag = typename T::DataType(1)) noexcept \
{ \
    T value; \
    value.raw()[(size) - 1] = sign mag; \
    return value; \
}

namespace LibMath
{
    namespace Internal
    {
        /// <summary>
        /// Defines static LinearMathArray construction functions based on directions
        /// </summary>
        template <typename T, ArrIndex = T::Size>
        struct DirectionAliases { };

        template <typename T>
        struct DirectionAliases<T, 1>
        {
            static constexpr T zero() noexcept
            {
                return T();
            }
        
            static constexpr T one(typename T::DataType mag = typename T::DataType(1)) noexcept
            {
                return T(mag);
            }

            DIRECTION_ALIAS_FUNC(left, 1, -)
            DIRECTION_ALIAS_FUNC(right, 1, +)
        };

        template <typename T>
        struct DirectionAliases<T, 2> : DirectionAliases<T, 1>
        {
            DIRECTION_ALIAS_FUNC(up, 2, +)
            DIRECTION_ALIAS_FUNC(down, 2, -)
        };

        template <typename T>
        struct DirectionAliases<T, 3> : DirectionAliases<T, 2>
        {
            DIRECTION_ALIAS_FUNC(front, 3, +)
            DIRECTION_ALIAS_FUNC(back, 3, -)
        };

        template <typename T>
        struct DirectionAliases<T, 4> : DirectionAliases<T, 3>
        {
            DIRECTION_ALIAS_FUNC(hyperUp, 4, +)
            DIRECTION_ALIAS_FUNC(hyperDown, 4, -)
        };
    }
}

// Cleanup
#undef DIRECTION_ALIAS_FUNC
