#pragma  once

#include "../Concepts.h"

#define POINT_ALIAS_IMPL(size, name) \
template <typename TData, ptrdiff_t Offset> \
/* Inherit from the previous size to progressively add aliases*/ \
/* Using __declspec to force EBO optimization as a workaround for a long-running MSVC bug*/\
struct __declspec(empty_bases) PointAliases<size, TData, Offset> : PointAliases<(size) - 1, TData, Offset> \
{ \
    _NODISCARD TData& name() noexcept \
    { \
        void* const location = reinterpret_cast<int8_t*>(this) + Offset;\
        return static_cast<TData*>(location)[(size) - 1]; \
    } \
    _NODISCARD const TData& name() const noexcept { return const_cast<PointAliases*>(this)->name(); } \
    private: \
        using _dataType = TData; \
        static constexpr ArrIndex _size = size; \
        static constexpr ArrIndex _offset = Offset; \
};

namespace LibMath::Internal
{
    /// <summary>
    /// Helper struct to dynamically define xyzw aliases for point-like types
    /// </summary>
    template <ArrIndex, ArrayData, ptrdiff_t = 0>
    struct __declspec(empty_bases) PointAliases { };

    POINT_ALIAS_IMPL(1, x)
    POINT_ALIAS_IMPL(2, y)
    POINT_ALIAS_IMPL(3, z)
    POINT_ALIAS_IMPL(4, w)
}

// Cleanup
#undef POINT_ALIAS_IMPL
